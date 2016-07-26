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

#ifndef THIRD_PARTY_CLD_3_SRC_SRC_BASE_H_
#define THIRD_PARTY_CLD_3_SRC_SRC_BASE_H_

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace chrome_lang_id {

using std::vector;
using std::string;
using std::map;
using std::pair;
typedef unsigned int uint32;

#if LANG_CXX11
#define CLD3_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName &) = delete;          \
  TypeName &operator=(const TypeName &) = delete
#else  // C++98 case follows

// Note that these C++98 implementations cannot completely disallow copying,
// as members and friends can still accidentally make elided copies without
// triggering a linker error.
#define CLD3_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName &);                   \
  TypeName &operator=(const TypeName &)
#endif  // LANG_CXX11

#define CLD3_CHECK(condition) assert(condition)
#define CLD3_CHECK_EQ(val1, val2) assert((val1) == (val2))
#define CLD3_CHECK_NE(val1, val2) assert((val1) != (val2))
#define CLD3_CHECK_LE(val1, val2) assert((val1) <= (val2))
#define CLD3_CHECK_LT(val1, val2) assert((val1) < (val2))
#define CLD3_CHECK_GE(val1, val2) assert((val1) >= (val2))
#define CLD3_CHECK_GT(val1, val2) assert((val1) > (val2))

#ifndef SWIG
typedef int int32;
typedef unsigned char uint8;    // NOLINT
typedef unsigned short uint16;  // NOLINT
#endif                          // SWIG

#ifdef COMPILER_MSVC
typedef __int64 int64;
#else
typedef long long int64;  // NOLINT
#endif  // COMPILER_MSVC

#if defined(__GNUC__) && \
    (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))

// For functions we want to force inline.
// Introduced in gcc 3.1.
#define CLD3_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))

#elif defined(_MSC_VER)
#define CLD3_ATTRIBUTE_ALWAYS_INLINE __forceinline
#else

// Other compilers will have to figure it out for themselves.
#define CLD3_ATTRIBUTE_ALWAYS_INLINE
#endif

#ifdef INTERNAL_BUILD
typedef basic_string<char> bstring;
#else
typedef std::basic_string<char> bstring;
#endif  // INTERNAL_BUILD

// Converts int64 to string.
std::string Int64ToString(int64 input);

}  // namespace chrome_lang_id

#endif  // THIRD_PARTY_CLD_3_SRC_SRC_BASE_H_
