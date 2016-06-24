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

#include "third_party/cld_3/src/embedding_network.h"

#include <iostream>
#include <memory>

#include "base/logging.h"
#include "third_party/cld_3/src/simple_adder.h"
#include "third_party/cld_3/src/task_context_params.h"

namespace chrome_lang_id {
namespace {

// Fills a Matrix object with the parameters in the given MatrixParams.
void FillMatrixParams(int num_rows, int num_cols, const float *weights,
                      EmbeddingNetwork::Matrix *mat) {
  mat->resize(num_rows);
  for (int r = 0; r < num_rows; ++r) {
    (*mat)[r] = EmbeddingNetwork::ConstVector(weights, num_cols);
    weights += num_cols;
  }
}

// Computes y = weights * Relu(x) + b where Relu is optionally applied.
template <typename ScaleAdderClass>
void SparseReluProductPlusBias(bool apply_relu,
                               const EmbeddingNetwork::Matrix &weights,
                               const EmbeddingNetwork::ConstVector &b,
                               const EmbeddingNetwork::Vector &x,
                               EmbeddingNetwork::Vector *y) {
  y->assign(b.data(), b.data() + b.size());
  ScaleAdderClass adder(y->data(), y->size());

  const int x_size = x.size();
  for (int i = 0; i < x_size; ++i) {
    const float &scale = x[i];
    if (apply_relu) {
      if (scale > 0) {
        adder.LazyScaleAdd(weights[i].data(), scale);
      }
    } else {
      adder.LazyScaleAdd(weights[i].data(), scale);
    }
  }
  adder.Finalize();
}

}  // namespace

void EmbeddingNetwork::ConcatEmbeddings(
    const vector<vector<SparseFeatures>> &sparse_features,
    Vector *concat) const {
  concat->resize(model_.concat_layer_size());
  for (size_t i = 0; i < sparse_features.size(); ++i) {
    int feature_offset = model_.concat_offset(i);
    for (size_t j = 0; j < sparse_features[i].size(); ++j) {
      for (int k = 0; k < sparse_features[i][j].id_size(); k++) {
        const int id = sparse_features[i][j].id(k);
        if (sparse_features[i][j].weight_size() > 0) {
          for (int embedding_offset = 0;
               embedding_offset < model_.embedding_dim(i); ++embedding_offset) {
            (*concat)[feature_offset + embedding_offset] +=
                embedding_weights_[i][id].data()[embedding_offset] *
                sparse_features[i][j].weight(k);
          }
        } else {
          for (int embedding_offset = 0;
               embedding_offset < model_.embedding_dim(i); ++embedding_offset) {
            (*concat)[feature_offset + embedding_offset] +=
                embedding_weights_[i][id].data()[embedding_offset];
          }
        }
      }
      feature_offset += model_.embedding_dim(i);
    }
  }
}

template <typename ScaleAdderClass>
void EmbeddingNetwork::FinishComputeFinalScores(const Vector &concat,
                                                Vector *scores) const {
  scores->resize(softmax_bias_.size(), 0.0);
  Vector h0(hidden_bias_[0].size());
  SparseReluProductPlusBias<ScaleAdderClass>(false, hidden_weights_[0],
                                             hidden_bias_[0], concat, &h0);

  if (hidden_weights_.size() == 1) {  // 1 hidden layer
    SparseReluProductPlusBias<ScaleAdderClass>(true, softmax_weights_,
                                               softmax_bias_, h0, scores);
  } else if (hidden_weights_.size() == 2) {  // 2 hidden layers
    Vector h1(hidden_bias_[1].size());
    SparseReluProductPlusBias<ScaleAdderClass>(true, hidden_weights_[1],
                                               hidden_bias_[1], h0, &h1);
    SparseReluProductPlusBias<ScaleAdderClass>(true, softmax_weights_,
                                               softmax_bias_, h1, scores);
  } else {
    LOG(FATAL) << ">2 hidden layers not supported.";
  }
}

void EmbeddingNetwork::ComputeFinalScores(
    const vector<vector<SparseFeatures>> &sparse_features, Vector *scores) {
  Vector concat;
  ConcatEmbeddings(sparse_features, &concat);
  FinishComputeFinalScores<SimpleAdder>(concat, scores);
}

EmbeddingNetwork::EmbeddingNetwork() {
  int offset_sum = 0;
  for (int i = 0; i < model_.embedding_dim_size(); ++i) {
    CHECK_EQ(offset_sum, model_.concat_offset(i)) << "Mismatch in model dims.";
    offset_sum += model_.embedding_dim(i) * model_.embedding_num_features(i);

    embedding_weights_.emplace_back();
    FillMatrixParams(model_.embeddings_num_rows(i),
                     model_.embeddings_num_cols(i),
                     model_.embeddings_weights(i), &embedding_weights_[i]);
  }

  CHECK_EQ(model_.hidden_size(), model_.hidden_bias_size());
  hidden_weights_.resize(model_.hidden_size());
  hidden_bias_.resize(model_.hidden_size());
  for (int i = 0; i < model_.hidden_size(); ++i) {
    FillMatrixParams(model_.hidden_num_rows(i), model_.hidden_num_cols(i),
                     model_.hidden_weights(i), &hidden_weights_[i]);

    CHECK_EQ(1, model_.hidden_bias_num_cols(i));
    hidden_bias_[i] = ConstVector(model_.hidden_bias_weights(i),
                                  model_.hidden_bias_num_rows(i));
  }

  FillMatrixParams(model_.softmax_num_rows(0), model_.softmax_num_cols(0),
                   model_.softmax_weights(0), &softmax_weights_);

  CHECK_EQ(1, model_.softmax_bias_size());
  CHECK_EQ(1, model_.softmax_bias_num_cols(0));
  softmax_bias_ = ConstVector(model_.softmax_bias_weights(0),
                              model_.softmax_bias_num_rows(0));
}

}  // namespace chrome_lang_id
