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

#include "language_identifier_features.h"

#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base.h"
#include "feature_extractor.h"
#include "feature_types.h"
#include "sentence_features.h"
#include "task_context.h"
#include "utils.h"

namespace chrome_lang_id {
NumericFeatureType::NumericFeatureType(const string &name, FeatureValue size)
    : FeatureType(name), size_(size) {}

string NumericFeatureType::GetFeatureValueName(FeatureValue value) const {
  return value < 0 ? "" : Int64ToString(value);
}

FeatureValue NumericFeatureType::GetDomainSize() const { return size_; }

void ContinuousBagOfNgramsFunction::Setup(TaskContext *context) {
  // Parameters in the feature function descriptor.
  include_terminators_ = GetBoolParameter("include_terminators", false);
  include_spaces_ = GetBoolParameter("include_spaces", false);
  use_equal_ngram_weight_ = GetBoolParameter("use_equal_weight", false);
  ngram_id_dimension_ = GetIntParameter("id_dim", 10000);
  ngram_size_ = GetIntParameter("size", 3);
}

void ContinuousBagOfNgramsFunction::Init(TaskContext *context) {
  set_feature_type(new NumericFeatureType(name(), ngram_id_dimension_));
}

void ContinuousBagOfNgramsFunction::Evaluate(const WorkspaceSet &workspaces,
                                             const Sentence &sentence,
                                             FeatureVector *result) const {
  // Include terminators for each token. Tokens are discovered by splitting the
  // text on spaces.
  vector<string> chars;
  utils::GetUTF8Chars(sentence.text(), &chars);
  if (include_terminators_) {
    vector<string> new_chars{"^"};
    for (size_t index = 0; index < chars.size(); ++index) {
      if (chars.at(index) == " ") {
        new_chars.push_back("$");
        new_chars.push_back(" ");
        new_chars.push_back("^");
      } else {
        new_chars.push_back(chars.at(index));
      }
    }
    new_chars.push_back("$");
    chars.swap(new_chars);
  }

  // Find the char ngram counts.
  std::unordered_map<string, int> char_ngram_counts;
  int count_sum = 0;
  for (int start = 0; start <= static_cast<int>(chars.size()) - ngram_size_;
       ++start) {
    string char_ngram;
    int index;
    for (index = 0; index < ngram_size_; ++index) {
      const string &current_char = chars.at(start + index);
      if (current_char == " " && !include_spaces_) {
        break;
      }
      char_ngram.append(current_char);
    }
    if (index == ngram_size_) {
      char_ngram_counts[char_ngram]++;
      ++count_sum;
    }
  }

  // Populate the feature vector.
  const float equal_weight = 1.0 / char_ngram_counts.size();
  const float norm = static_cast<float>(count_sum);
  for (const auto &ngram_and_count : char_ngram_counts) {
    const float weight =
        use_equal_ngram_weight_ ? equal_weight : ngram_and_count.second / norm;
    FloatFeatureValue value(
        utils::Hash32WithDefaultSeed(ngram_and_count.first) %
            ngram_id_dimension_,
        weight);
    result->add(feature_type(), value.discrete_value);
  }
}

}  // namespace chrome_lang_id
