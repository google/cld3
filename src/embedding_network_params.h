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

#ifndef THIRD_PARTY_CLD_3_SRC_EMBEDDING_NETWORK_PARAMS_H_
#define THIRD_PARTY_CLD_3_SRC_EMBEDDING_NETWORK_PARAMS_H_

#include "third_party/cld_3/src/base.h"

namespace chrome_lang_id {

class EmbeddingNetworkParams {
 public:
  // Access methods for embeddings:
  int embeddings_size() const { return 4; }
  int embeddings_num_rows(int i) const { return kEmbeddingsNumRows[i]; }
  int embeddings_num_cols(int i) const { return kEmbeddingsNumCols[i]; }
  const float *embeddings_weights(int i) const {
    return embeddings_weights_[i];
  }

  // Access methods for hidden:
  int hidden_size() const { return 1; }
  int hidden_num_rows(int i) const { return kHiddenNumRows[i]; }
  int hidden_num_cols(int i) const { return kHiddenNumCols[i]; }
  const float *hidden_weights(int i) const { return hidden_weights_[i]; }

  // Access methods for hidden_bias:
  int hidden_bias_size() const { return 1; }
  int hidden_bias_num_rows(int i) const { return kHiddenBiasNumRows[i]; }
  int hidden_bias_num_cols(int i) const { return kHiddenBiasNumCols[i]; }
  const float *hidden_bias_weights(int i) const {
    return hidden_bias_weights_[i];
  }

  // Access methods for softmax:
  int softmax_size() const { return 1; }
  int softmax_num_rows(int i) const { return kSoftmaxNumRows[i]; }
  int softmax_num_cols(int i) const { return kSoftmaxNumCols[i]; }
  const float *softmax_weights(int i) const { return softmax_weights_[i]; }

  // Access methods for softmax_bias:
  int softmax_bias_size() const { return 1; }
  int softmax_bias_num_rows(int i) const { return kSoftmaxBiasNumRows[i]; }
  int softmax_bias_num_cols(int i) const { return kSoftmaxBiasNumCols[i]; }
  const float *softmax_bias_weights(int i) const {
    return softmax_bias_weights_[i];
  }

  // Access methods for embedding_dim:
  int embedding_dim_size() const { return 4; }
  int32 embedding_dim(int i) const { return kEmbeddingDimValues[i]; }

  // Access methods for embedding_num_features:
  int embedding_num_features_size() const { return 4; }
  int32 embedding_num_features(int i) const {
    return kEmbeddingNumFeaturesValues[i];
  }

  // Access methods for embedding_features_domain_size:
  int embedding_features_domain_size_size() const { return 4; }
  int32 embedding_features_domain_size(int i) const {
    return kEmbeddingFeaturesDomainSizeValues[i];
  }

  // Access methods for concat_offset:
  int concat_offset_size() const { return 4; }
  int32 concat_offset(int i) const { return kConcatOffsetValues[i]; }

  // Access methods for concat_layer_size:
  bool has_concat_layer_size() const { return true; }
  int32 concat_layer_size() const { return 64; }

  // Access methods for is_precomputed:
  bool has_is_precomputed() const { return false; }
  bool is_precomputed() const { return false; }

 private:
  // Private fields for embeddings:
  static const int kEmbeddingsNumRows[];
  static const int kEmbeddingsNumCols[];
  static const float kEmbeddingsWeights0[];
  static const float kEmbeddingsWeights1[];
  static const float kEmbeddingsWeights2[];
  static const float kEmbeddingsWeights3[];
  const float *embeddings_weights_[4] = {
      kEmbeddingsWeights0, kEmbeddingsWeights1, kEmbeddingsWeights2,
      kEmbeddingsWeights3};

  // Private fields for hidden:
  static const int kHiddenNumRows[];
  static const int kHiddenNumCols[];
  static const float kHiddenWeights0[];
  const float *hidden_weights_[1] = {kHiddenWeights0};

  // Private fields for hidden_bias:
  static const int kHiddenBiasNumRows[];
  static const int kHiddenBiasNumCols[];
  static const float kHiddenBiasWeights0[];
  const float *hidden_bias_weights_[1] = {kHiddenBiasWeights0};

  // Private fields for softmax:
  static const int kSoftmaxNumRows[];
  static const int kSoftmaxNumCols[];
  static const float kSoftmaxWeights0[];
  const float *softmax_weights_[1] = {kSoftmaxWeights0};

  // Private fields for softmax_bias:
  static const int kSoftmaxBiasNumRows[];
  static const int kSoftmaxBiasNumCols[];
  static const float kSoftmaxBiasWeights0[];
  const float *softmax_bias_weights_[1] = {kSoftmaxBiasWeights0};

  // Private fields for embedding_dim:
  static const int32 kEmbeddingDimValues[];

  // Private fields for embedding_num_features:
  static const int32 kEmbeddingNumFeaturesValues[];

  // Private fields for embedding_features_domain_size:
  static const int32 kEmbeddingFeaturesDomainSizeValues[];

  // Private fields for concat_offset:
  static const int32 kConcatOffsetValues[];
};  // class EmbeddingNetworkParams

}  // namespace chrome_lang_id

#endif  // THIRD_PARTY_CLD_3_SRC_EMBEDDING_NETWORK_PARAMS_H_
