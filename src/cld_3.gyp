# Copyright 2016 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# =============================================================================*/

{
  'targets': [
    {
      'target_name': 'cld_3',
      'type': 'static_library',
      'includes': ['../../../../build/protoc.gypi'],
      'sources': [
        'feature_extractor.proto',
        'sentence.proto',
        'task_spec.proto',
        'base.h',
        'base.cc',
        'casts.h',
        'embedding_feature_extractor.h',
        'embedding_feature_extractor.cc',
        'embedding_network.h',
        'embedding_network.cc',
        'embedding_network_params.h',
        'feature_extractor.h',
        'feature_extractor.cc',
        'feature_types.h',
        'feature_types.cc',
        'float16.h',
        'fml_parser.h',
        'fml_parser.cc',
        'language_identifier_features.h',
        'language_identifier_features.cc',
        'lang_id_nn_params.h',
        'lang_id_nn_params.cc',
        'nnet_language_identifier.h',
        'nnet_language_identifier.cc',
        'registry.h',
        'registry.cc',
        'sentence_features.h',
        'sentence_features.cc',
        'simple_adder.h',
        'task_context.h',
        'task_context.cc',
        'task_context_params.h',
        'task_context_params.cc',
        'utils.h',
        'utils.cc',
        'workspace.h',
        'workspace.cc',
        'script_span/fixunicodevalue.cc',
        'script_span/generated_entities.cc',
        'script_span/generated_ulscript.cc',
        'script_span/getonescriptspan.cc',
        'script_span/offsetmap.cc',
        'script_span/utf8statetable.cc',
        'script_span/fixunicodevalue.h',
        'script_span/generated_ulscript.h',
        'script_span/getonescriptspan.h',
        'script_span/integral_types.h',
        'script_span/offsetmap.h',
        'script_span/port.h',
        'script_span/stringpiece.h',
        'script_span/utf8acceptinterchange.h',
        'script_span/utf8prop_lettermarkscriptnum.h',
        'script_span/utf8scannot_lettermarkspecial.h',
        'script_span/utf8statetable.h',
      ],
      'variables': {
        'proto_in_dir': './',
        'proto_out_dir': 'third_party/cld_3/src/src',
      },
    },
  ],
}
