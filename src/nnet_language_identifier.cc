/* Copyright 2016 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "third_party/cld_3/src/src/nnet_language_identifier.h"

#include <math.h>

#include <algorithm>
#include <limits>
#include <string>

#include "third_party/cld_3/src/src/base.h"
#include "third_party/cld_3/src/src/embedding_network.h"
#include "third_party/cld_3/src/src/script_span/generated_ulscript.h"
#include "third_party/cld_3/src/src/script_span/getonescriptspan.h"
#include "third_party/cld_3/src/src/sentence.pb.h"
#include "third_party/cld_3/src/src/task_context.h"
#include "third_party/cld_3/src/src/workspace.h"

namespace chrome_lang_id {
namespace {

// Struct for accumulating stats for a language as text subsequences of the same
// script are processed.
struct LangChunksStats {
  // Sum of probabilities across subsequences.
  float prob_sum = 0.0;

  // Whether the predictions for the subsequences are reliable.
  bool is_reliable = true;

  // Total number of bytes corresponding to the language.
  int byte_sum = 0;

  // Number chunks corresponding to the language.
  int num_chunks = 0;
};

// Compares two pairs based on their values.
bool OrderBySecondDescending(const std::pair<string, float> &x,
                             const std::pair<string, float> &y) {
  if (x.second == y.second) {
    return x.first < y.first;
  } else {
    return x.second > y.second;
  }
}

}  // namespace

const int NNetLanguageIdentifier::kMinNumBytesToConsider = 100;
const int NNetLanguageIdentifier::kMaxNumBytesToConsider = 1000;
const char NNetLanguageIdentifier::kUnknown[] = "<unknown>";
const float NNetLanguageIdentifier::kReliabilityThreshold = 0.53f;

const string LanguageIdEmbeddingFeatureExtractor::ArgPrefix() const {
  return "language_identifier";
}

NNetLanguageIdentifier::NNetLanguageIdentifier()
    : NNetLanguageIdentifier(kMinNumBytesToConsider, kMaxNumBytesToConsider) {}

NNetLanguageIdentifier::NNetLanguageIdentifier(int min_num_bytes,
                                               int max_num_bytes)
    : num_languages_(TaskContextParams::GetNumLanguages()),
      network_(&nn_params_),
      min_num_bytes_(min_num_bytes),
      max_num_bytes_(max_num_bytes) {
  // Get the model parameters, set up and initialize the model.
  TaskContext context;
  TaskContextParams::ToTaskContext(&context);
  Setup(&context);
  Init(&context);
}

NNetLanguageIdentifier::~NNetLanguageIdentifier() {}

void NNetLanguageIdentifier::Setup(TaskContext *context) {
  feature_extractor_.Setup(context);
}

void NNetLanguageIdentifier::Init(TaskContext *context) {
  feature_extractor_.Init(context);
  feature_extractor_.RequestWorkspaces(&workspace_registry_);
}

void NNetLanguageIdentifier::GetFeatures(
    Sentence *sentence, vector<FeatureVector> *features) const {
  // Feature workspace set.
  WorkspaceSet workspace;
  workspace.Reset(workspace_registry_);
  feature_extractor_.Preprocess(&workspace, sentence);
  feature_extractor_.ExtractFeatures(workspace, *sentence, features);
}

// Returns the language name corresponding to the given id.
string NNetLanguageIdentifier::GetLanguageName(int language_id) const {
  CLD3_CHECK((language_id >= 0) && (language_id < num_languages_));
  return TaskContextParams::language_names(language_id);
}

NNetLanguageIdentifier::Result NNetLanguageIdentifier::FindLanguage(
    const string &text) {
  const int num_bytes_to_process =
      std::min(static_cast<int>(text.size()), max_num_bytes_);
  const int num_valid_bytes =
      CLD2::SpanInterchangeValid(text.c_str(), num_bytes_to_process);
  if (num_valid_bytes < min_num_bytes_) {
    return Result();
  }

  std::string text_to_process(text.c_str(), num_valid_bytes);
  return FindLanguageOfValidUTF8(text_to_process);
}

NNetLanguageIdentifier::Result NNetLanguageIdentifier::FindLanguageOfValidUTF8(
    const string &text) {
  // Create a Sentence storing the input text.
  Sentence sentence;
  sentence.set_text(text);
  Token *token = sentence.add_token();
  token->set_word(text);
  token->set_start(0);
  token->set_end(text.length() - 1);

  // Predict language.
  // TODO(salcianu): reuse vector<FeatureVector>.
  vector<FeatureVector> features(feature_extractor_.NumEmbeddings());
  GetFeatures(&sentence, &features);

  EmbeddingNetwork::Vector scores;
  network_.ComputeFinalScores(features, &scores);
  int prediction_id = -1;
  float max_val = -std::numeric_limits<float>::infinity();
  for (size_t i = 0; i < scores.size(); ++i) {
    if (scores[i] > max_val) {
      prediction_id = i;
      max_val = scores[i];
    }
  }

  // Compute probability.
  Result result;
  float diff_sum = 0.0;
  for (size_t i = 0; i < scores.size(); ++i) {
    diff_sum += exp(scores[i] - max_val);
  }
  const float log_sum_exp = max_val + log(diff_sum);
  result.probability = exp(max_val - log_sum_exp);

  result.language = GetLanguageName(prediction_id);
  result.is_reliable = (result.probability >= kReliabilityThreshold);
  result.proportion = 1.0;
  return result;
}

std::vector<NNetLanguageIdentifier::Result>
NNetLanguageIdentifier::FindTopNMostLikelyLangs(const string &text,
                                                int num_langs) {
  std::vector<Result> results;
  const int num_valid_bytes =
      CLD2::SpanInterchangeValid(text.c_str(), text.size());
  if (num_valid_bytes == 0) {
    while (num_langs-- > 0) {
      results.emplace_back();
    }
    return results;
  }

  // Process each subsequence of the same script.
  CLD2::ScriptScanner ss(text.c_str(), num_valid_bytes, /*is_plain_text=*/true);
  CLD2::LangSpan script_span;
  std::unordered_map<string, LangChunksStats> lang_stats;
  int total_num_bytes = 0;
  Result result;
  string language;
  while (ss.GetOneScriptSpan(&script_span)) {
    if (script_span.text_bytes < min_num_bytes_) {
      continue;
    }
    total_num_bytes += script_span.text_bytes;

    const int num_bytes_to_process =
        std::min(script_span.text_bytes, max_num_bytes_);
    const int num_valid_bytes =
        CLD2::SpanInterchangeValid(script_span.text, num_bytes_to_process);
    const string span_text(script_span.text, num_valid_bytes);
    result = FindLanguageOfValidUTF8(span_text);
    language = result.language;
    lang_stats[language].byte_sum += script_span.text_bytes;
    lang_stats[language].prob_sum += result.probability;
    lang_stats[language].is_reliable =
        (lang_stats[language].is_reliable && result.is_reliable);
    lang_stats[language].num_chunks++;
  }

  // Sort the languages based on the average probability.
  // TODO(abakalov): Consider alternative possibly more efficient portable
  // approaches for finding the top N languages. Given that on average, there
  // aren't that many languages in the input, it's likely that the benefits will
  // be negligible (if any).
  std::vector<std::pair<string, float>> langs_and_avrg_probs;
  for (const auto &entry : lang_stats) {
    const float avrg_prob = entry.second.prob_sum / entry.second.num_chunks;
    langs_and_avrg_probs.emplace_back(entry.first, avrg_prob);
  }
  std::sort(langs_and_avrg_probs.begin(), langs_and_avrg_probs.end(),
            OrderBySecondDescending);

  const float byte_sum = static_cast<float>(total_num_bytes);
  const int num_langs_to_save =
      std::min(num_langs, static_cast<int>(langs_and_avrg_probs.size()));
  for (int indx = 0; indx < num_langs_to_save; ++indx) {
    Result result;
    const string &language = langs_and_avrg_probs.at(indx).first;
    result.language = language;
    result.probability = langs_and_avrg_probs.at(indx).second;
    result.is_reliable = lang_stats.at(language).is_reliable;
    result.proportion = lang_stats.at(language).byte_sum / byte_sum;
    results.push_back(result);
  }

  int padding_size = num_langs - langs_and_avrg_probs.size();
  while (padding_size-- > 0) {
    results.emplace_back();
  }
  return results;
}

}  // namespace chrome_lang_id
