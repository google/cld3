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

#include "third_party/cld_3/src/src/sentence_features.h"

#include "third_party/cld_3/src/src/registry.h"

namespace chrome_lang_id {

// Registry for the whole Sentence feature functions.
REGISTER_CLASS_REGISTRY("sentence feature function", WholeSentenceFeature);

}  // namespace chrome_lang_id
