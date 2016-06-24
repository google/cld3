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

#ifndef THIRD_PARTY_CLD_3_SRC_EMBEDDING_NETWORK_H_
#define THIRD_PARTY_CLD_3_SRC_EMBEDDING_NETWORK_H_

#include <vector>

#include "base/logging.h"
#include "third_party/cld_3/src/base.h"
#include "third_party/cld_3/src/embedding_network_params.h"
#include "third_party/cld_3/src/simple_adder.h"
#include "third_party/cld_3/src/sparse.pb.h"

namespace chrome_lang_id {

class EmbeddingNetwork {
 public:
  // An immutable vector that doesn't own the memory that stores the underlying
  // floats.  Can be used e.g., as a wrapper around model weights stored in the
  // static memory.
  class ConstVector {
   public:
    ConstVector() : ConstVector(nullptr, 0) {}
    ConstVector(const float *data, int size) : data_(data), size_(size) {}
    int size() const { return size_; }
    const float *data() const { return data_; }

   private:
    const float *data_;  // Not owned.
    int size_;
  };

  // Precision of the network.
  typedef std::vector<ConstVector> Matrix;
  typedef std::vector<float> Vector;

  // Constructs an embedding network.
  EmbeddingNetwork();

  virtual ~EmbeddingNetwork() {}

  // Runs forward computation to fill scores with unnormalized output unit
  // scores. This is useful for making predictions.
  void ComputeFinalScores(const vector<vector<SparseFeatures>> &sparse_features,
                          Vector *scores);

 private:
  // Computes the softmax scores (prior to normalization) from the concatenated
  // representation.
  template <typename ScaleAdderClass>
  void FinishComputeFinalScores(const Vector &concat, Vector *scores) const;

  // Constructs the concatenated input embedding vector in place in output
  // vector concat.
  void ConcatEmbeddings(const vector<vector<SparseFeatures>> &sparse_features,
                        Vector *concat) const;

  EmbeddingNetworkParams model_;

  // Network parameters.
  vector<Matrix> hidden_weights_;
  vector<ConstVector> hidden_bias_;
  Matrix softmax_weights_;
  ConstVector softmax_bias_;

  // Embedding parameters.
  vector<Matrix> embedding_weights_;
};

}  // namespace chrome_lang_id

#endif  // THIRD_PARTY_CLD_3_SRC_EMBEDDING_NETWORK_H_
