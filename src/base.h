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

#ifndef THIRD_PARTY_CLD_3_SRC_BASE_H_
#define THIRD_PARTY_CLD_3_SRC_BASE_H_

#include <map>
#include <string>
#include <vector>

#ifdef INTERNAL_BUILD
#include "strings/stringpiece.h"
#else
#include "base/strings/string_piece.h"
#endif  // INTERNAL_BUILD

namespace chrome_lang_id {

using std::vector;
using std::string;
using std::map;
using std::pair;
typedef unsigned int uint32;

#ifndef SWIG
typedef int int32;
#endif  // SWIG

#ifdef COMPILER_MSVC
typedef __int64 int64;
#else
typedef long long int64;  // NOLINT
#endif  // COMPILER_MSVC

#ifdef INTERNAL_BUILD
typedef basic_string<char> bstring;
#else
using base::StringPiece;
typedef std::basic_string<char> bstring;

// For functions we want to force inline.
// Introduced in gcc 3.1.
#define ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#endif  // INTERNAL_BUILD

// Converts int64 to string.
std::string Int64ToString(int64 input);

}  // namespace chrome_lang_id

#endif  // THIRD_PARTY_CLD_3_SRC_BASE_H_
