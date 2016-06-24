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

// Features that operate on Sentence objects. Most features are defined
// in this header so they may be re-used via composition into other more
// advanced feature classes.

#ifndef THIRD_PARTY_CLD_3_SRC_SENTENCE_FEATURES_H_
#define THIRD_PARTY_CLD_3_SRC_SENTENCE_FEATURES_H_

#include "third_party/cld_3/src/feature_extractor.h"
#include "third_party/cld_3/src/sentence.pb.h"

namespace chrome_lang_id {

// Feature function for any component that processes Sentences, whose
// focus is a token index into the sentence.
typedef FeatureFunction<Sentence, int> SentenceFeature;

typedef FeatureExtractor<Sentence, int> SentenceExtractor;

// Utility to register the sentence_instance::Feature functions.
#define REGISTER_SENTENCE_IDX_FEATURE(name, type) \
  REGISTER_FEATURE_FUNCTION(SentenceFeature, name, type)

}  // namespace chrome_lang_id

#endif  // THIRD_PARTY_CLD_3_SRC_SENTENCE_FEATURES_H_
