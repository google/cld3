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

#include "nnet_language_identifier.h"

#include <math.h>

#include <algorithm>
#include <limits>
#include <string>

#include "base.h"
#include "embedding_network.h"
#include "registry.h"
#include "script_span/generated_ulscript.h"
#include "script_span/getonescriptspan.h"
#include "script_span/text_processing.h"
#include "cld_3/protos/sentence.pb.h"
#include "sentence_features.h"
#include "task_context.h"
#include "workspace.h"

namespace chrome_lang_id {
namespace {

// Struct for accumulating stats for a language as text subsequences of the same
// script are processed.
struct LangChunksStats {
  // Sum of probabilities across subsequences.
  float prob_sum = 0.0;

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

const int NNetLanguageIdentifier::kMinNumBytesToConsider = 140;
const int NNetLanguageIdentifier::kMaxNumBytesToConsider = 512;
const int NNetLanguageIdentifier::kMaxNumInputBytesToConsider = 10000;
const char NNetLanguageIdentifier::kUnknown[] = "unknown";
const float NNetLanguageIdentifier::kReliabilityThreshold = 0.7f;
const float NNetLanguageIdentifier::kReliabilityHrBsThreshold = 0.5f;

const string LanguageIdEmbeddingFeatureExtractor::ArgPrefix() const {
  return "language_identifier";
}

NNetLanguageIdentifier::NNetLanguageIdentifier()
    : NNetLanguageIdentifier(kMinNumBytesToConsider, kMaxNumBytesToConsider) {}

static WholeSentenceFeature *cbog_factory() {
  return new ContinuousBagOfNgramsFunction;
}

NNetLanguageIdentifier::NNetLanguageIdentifier(int min_num_bytes,
                                               int max_num_bytes)
    : num_languages_(TaskContextParams::GetNumLanguages()),
      network_(&nn_params_),
      min_num_bytes_(min_num_bytes),
      max_num_bytes_(max_num_bytes) {
  if (WholeSentenceFeature::registry() == nullptr) {
    // Create registry for our WholeSentenceFeature(s).
    RegisterableClass<WholeSentenceFeature>::CreateRegistry(
        "sentence feature function", "WholeSentenceFeature", __FILE__,
        __LINE__);
  }

  // Register our WholeSentenceFeature(s).
  // Register ContinuousBagOfNgramsFunction feature function.
  static WholeSentenceFeature::Registry::Registrar cbog_registrar(
      WholeSentenceFeature::registry(), "continuous-bag-of-ngrams",
      "ContinuousBagOfNgramsFunction", __FILE__, __LINE__, cbog_factory);

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
  CLD3_CHECK(language_id >= 0);
  CLD3_CHECK(language_id < num_languages_);
  return TaskContextParams::language_names(language_id);
}

NNetLanguageIdentifier::Result NNetLanguageIdentifier::FindLanguage(
    const string &text) {
  const int num_bytes_to_process =
      std::min(static_cast<int>(text.size()), max_num_bytes_);
  const int num_valid_bytes =
      CLD2::SpanInterchangeValid(text.c_str(), num_bytes_to_process);

  // Iterate over the input with ScriptScanner to clean up the text (e.g.,
  // removing digits, punctuation, brackets).
  // TODO(abakalov): Extract the code that does the clean-up out of
  // ScriptScanner.
  CLD2::ScriptScanner ss(text.c_str(), num_valid_bytes, /*is_plain_text=*/true);
  CLD2::LangSpan script_span;
  std::string cleaned;
  while (ss.GetOneScriptSpan(&script_span)) {
    // script_span has spaces at the beginning and the end, so there is no need
    // for a delimiter.
    cleaned.append(script_span.text, script_span.text_bytes);
  }

  if (static_cast<int>(cleaned.size()) < min_num_bytes_) {
    return Result();
  }

  // Copy to a vector because a non-const char* will be needed.
  std::vector<char> text_to_process;
  for (size_t i = 0; i < cleaned.size(); ++i) {
    text_to_process.push_back(cleaned[i]);
  }
  text_to_process.push_back('\0');

  // Remove repetitive chunks or ones containing mostly spaces.
  const int chunk_size = 0;  // Use the default.
  char *text_start = &text_to_process[0];
  const int new_length = CLD2::CheapSqueezeInplace(
      text_start, text_to_process.size() - 1, chunk_size);
  if (new_length < min_num_bytes_) {
    return Result();
  }

  std::string squeezed_text_to_process(text_start, new_length);
  return FindLanguageOfValidUTF8(squeezed_text_to_process);
}

NNetLanguageIdentifier::Result NNetLanguageIdentifier::FindLanguageOfValidUTF8(
    const string &text) {
  // Create a Sentence storing the input text.
  Sentence sentence;
  sentence.set_text(text);

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
NNetLanguageIdentifier::FindTopNMostFreqLangs(const string &text,
                                              int num_langs) {
  std::vector<Result> results;

  // Truncate the input text if it is too long and find the span containing
  // interchange-valid UTF8.
  const int num_valid_bytes = CLD2::SpanInterchangeValid(
      text.c_str(),
      std::min(kMaxNumInputBytesToConsider, static_cast<int>(text.size())));
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
  int chunk_size = 0;  // Use the default.
  while (ss.GetOneScriptSpan(&script_span)) {
    const int num_original_span_bytes = script_span.text_bytes;

    // Remove repetitive chunks or ones containing mostly spaces.
    const int new_length = CLD2::CheapSqueezeInplace(
        script_span.text, script_span.text_bytes, chunk_size);
    script_span.text_bytes = new_length;

    if (script_span.text_bytes < min_num_bytes_) {
      continue;
    }
    total_num_bytes += num_original_span_bytes;

    // If the text is longer than max_num_bytes_, find the middle snippet of
    // length max_num_bytes_.
    std::string span_text;
    if (script_span.text_bytes > max_num_bytes_) {
      // Offset of the middle snippet. Using SpanInterchangeValid to ensure that
      // we are not splitting a character in two. This function is used on the
      // following line for the same reason.
      const int offset = CLD2::SpanInterchangeValid(
          script_span.text, (script_span.text_bytes - max_num_bytes_) / 2);
      const int num_valid_snippet_bytes =
          CLD2::SpanInterchangeValid(script_span.text + offset, max_num_bytes_);
      const std::string middle_snippet(script_span.text + offset,
                                       num_valid_snippet_bytes);
      span_text = middle_snippet;
    } else {
      const std::string snippet(script_span.text, script_span.text_bytes);
      span_text = snippet;
    }

    result = FindLanguageOfValidUTF8(span_text);
    language = result.language;
    lang_stats[language].byte_sum += num_original_span_bytes;
    lang_stats[language].prob_sum +=
        result.probability * num_original_span_bytes;
    lang_stats[language].num_chunks++;
  }

  // Sort the languages based on the number of bytes associated with them.
  // TODO(abakalov): Consider alternative possibly more efficient portable
  // approaches for finding the top N languages. Given that on average, there
  // aren't that many languages in the input, it's likely that the benefits will
  // be negligible (if any).
  std::vector<std::pair<string, float>> langs_and_byte_counts;
  for (const auto &entry : lang_stats) {
    langs_and_byte_counts.emplace_back(entry.first, entry.second.byte_sum);
  }
  std::sort(langs_and_byte_counts.begin(), langs_and_byte_counts.end(),
            OrderBySecondDescending);

  const float byte_sum = static_cast<float>(total_num_bytes);
  const int num_langs_to_save =
      std::min(num_langs, static_cast<int>(langs_and_byte_counts.size()));
  for (int indx = 0; indx < num_langs_to_save; ++indx) {
    Result result;
    const string &language = langs_and_byte_counts.at(indx).first;
    const LangChunksStats &stats = lang_stats.at(language);
    result.language = language;
    result.probability = stats.prob_sum / stats.byte_sum;
    result.proportion = stats.byte_sum / byte_sum;

    if (language == "hr" || language == "bs") {
      result.is_reliable = (result.probability >= kReliabilityHrBsThreshold);
    } else {
      result.is_reliable = (result.probability >= kReliabilityThreshold);
    }
    results.push_back(result);
  }

  int padding_size = num_langs - langs_and_byte_counts.size();
  while (padding_size-- > 0) {
    results.emplace_back();
  }
  return results;
}

}  // namespace chrome_lang_id
