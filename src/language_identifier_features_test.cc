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

#include <cmath>
#include <iostream>
#include <vector>

#include "base.h"
#include "feature_extractor.h"
#include "language_identifier_features.h"
#include "nnet_language_identifier.h"
#include "cld_3/protos/sentence.pb.h"
#include "task_context.h"
#include "utils.h"
#include "workspace.h"

namespace chrome_lang_id {
namespace language_identifier_features_test {

static WholeSentenceFeature *cbog_factory() {
  return new ContinuousBagOfNgramsFunction;
}

// Class for calculating the feature weights and ids.
class FeatureIdWeightCalculator {
 public:
  explicit FeatureIdWeightCalculator(TaskContext *context) {
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

    sentence_.set_text("aa aab");
    feature_extractor_.Setup(context);
    feature_extractor_.Init(context);
  }

  // Returns a map from feature value id to feature value weight.
  std::unordered_map<int, float> GetFeatureValIdsAndWeights() {
    WorkspaceSet workspace;
    workspace.Reset(workspace_registry_);
    feature_extractor_.Preprocess(&workspace, &sentence_);

    const int num_feature_spaces = 1;  // only ngrams
    vector<FeatureVector> features(num_feature_spaces);
    feature_extractor_.ExtractFeatures(workspace, sentence_, &features);
    CLD3_CHECK(features.size() == num_feature_spaces);
    const FeatureVector &ngram_features = features.at(0);

    // Save the (feature value id, feature value weight) pairs to a map.
    std::unordered_map<int, float> feature_id_weight;
    for (int index = 0; index < ngram_features.size(); ++index) {
      const FloatFeatureValue feature_value =
          FloatFeatureValue(ngram_features.value(index));
      feature_id_weight[feature_value.value.id] = feature_value.value.weight;
    }
    return feature_id_weight;
  }

 private:
  // The registry of shared workspaces in the feature extractor.
  WorkspaceRegistry workspace_registry_;
  LanguageIdEmbeddingFeatureExtractor feature_extractor_;
  Sentence sentence_;
};

// Extracts features and checks that their ids and weights are correct.
bool ExtractAndCheckFeatures(const string &features, const int id_dim,
                             const vector<string> &expected_char_ngrams,
                             const vector<float> &expected_weights) {
  TaskContext context;
  context.SetParameter("language_identifier_features", features);
  FeatureIdWeightCalculator calc(&context);

  // Get the feature ids and the corresponding weights.
  const std::unordered_map<int, float> feature_id_weight =
      calc.GetFeatureValIdsAndWeights();
  if (feature_id_weight.size() != expected_char_ngrams.size()) {
    std::cout << "  Failure" << std::endl;
    std::cout << "  Number of expected feature ids: "
              << expected_char_ngrams.size() << std::endl;
    std::cout << "  Number of extracted feature ids: "
              << feature_id_weight.size() << std::endl;
    return false;
  }

  // Specifies how close two float values should be to be considered equal.
  const float epsilon = 0.0001f;
  bool test_successful = true;
  for (size_t i = 0; i < expected_char_ngrams.size(); ++i) {
    const int expected_id =
        utils::Hash32WithDefaultSeed(expected_char_ngrams.at(i)) % id_dim;

    // Check the ids and the weights.
    if (feature_id_weight.count(expected_id) == 0) {
      std::cout << "  Failure" << std::endl;
      std::cout << "  Feature id " << expected_id << " is missing" << std::endl;
      test_successful = false;
    } else {
      if (std::abs(feature_id_weight.at(expected_id) - expected_weights.at(i)) >
          epsilon) {
        std::cout << "  Failure" << std::endl;
        std::cout << "  Different weight for feature id " << expected_id
                  << ": expected weight " << expected_weights.at(i)
                  << ", actual weight " << feature_id_weight.at(expected_id)
                  << std::endl;
        test_successful = false;
      }
    }
  }

  if (test_successful) {
    std::cout << "  Success!" << std::endl;
  }
  return test_successful;
}

// Tests the case when ngram features get equal weight. Returns "true" if the
// test is successful and "false" otherwise.
bool TestExtractFeaturesWithEqualWeight() {
  std::cout << "Running " << __FUNCTION__ << std::endl;

  // The integer id of each char ngram is computed as follows:
  // utils::Hash32WithDefaultSeed(char ngram) % id_dim.
  const int id_dim = 100;
  const string features = "continuous-bag-of-ngrams(id_dim=" +
                          std::to_string(id_dim) +
                          ",size=2,include_terminators=true,include_" +
                          "spaces=false,use_equal_weight=true)";

  const vector<string> expected_char_ngrams{"ab", "b$", "^a", "aa", "a$"};
  const vector<float> expected_weights = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
  return ExtractAndCheckFeatures(features, id_dim, expected_char_ngrams,
                                 expected_weights);
}

// Tests the case when ngram features get weights equal to their normalized
// counts. Returns "true" if the test is successful and "false" otherwise.
bool TestExtractFeaturesWithNonEqualWeight() {
  std::cout << "Running " << __FUNCTION__ << std::endl;

  // The integer id of each char ngram is computed as follows:
  // utils::Hash32WithDefaultSeed(char ngram) % id_dim.
  const int id_dim = 100;
  const string features = "continuous-bag-of-ngrams(id_dim=" +
                          std::to_string(id_dim) +
                          ",size=2,include_terminators=true,include_" +
                          "spaces=false,use_equal_weight=false)";
  const vector<string> expected_char_ngrams{"ab", "b$", "^a", "aa", "a$"};
  const vector<float> expected_weights{0.1428f, 0.1428f, 0.2857f, 0.2857f,
                                       0.1428f};
  return ExtractAndCheckFeatures(features, id_dim, expected_char_ngrams,
                                 expected_weights);
}

}  // namespace language_identifier_features_test
}  // namespace chrome_lang_id

// Runs the feature extraction tests.
int main(int argc, char **argv) {
  const bool tests_successful =
      chrome_lang_id::language_identifier_features_test::
          TestExtractFeaturesWithEqualWeight() &&
      chrome_lang_id::language_identifier_features_test::
          TestExtractFeaturesWithNonEqualWeight();
  return tests_successful ? 0 : 1;
}
