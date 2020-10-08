#ifndef LIBDLI_HASH_H_
#define LIBDLI_HASH_H_
/*
* Copyright (c) 2020 Samsung Electronics Co., Ltd.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

// INTERNAL INCLUDES
#include "libdli-api.h"

// EXTERNAL INCLUDES
#include <string>
#include <cstring>
#include <cstdint>

namespace
{

class LIBDLI_API Hash
{
public:
  static constexpr uint64_t DEFAULT_SEED = 61081;

  Hash(uint64_t initial = DEFAULT_SEED)
  :  mValue(initial)
  {}

  Hash& Add(bool b)
  {
    mValue = Concatenate(b ? 0 : 1);
    return *this;
  }

  Hash& Add(int32_t i)
  {
    mValue = Concatenate(i);
    return *this;
  }

  Hash& Add(uint32_t i)
  {
    mValue = Concatenate(i);
    return *this;
  }

  Hash& Add(uint64_t i)
  {
    mValue = Concatenate(i);
    return *this;
  }

  Hash& Add(float f)
  {
    return AddObjectBytes(f);
  }

  Hash& Add(const char* cStr)
  {
    return Add(cStr, strlen(cStr));
  }

  Hash& Add(const char* cStr, size_t len)
  {
    auto i0 = reinterpret_cast<const uint8_t*>(cStr);
    return AddBytes(i0, i0 + len);
  }

  Hash& Add(const std::string& str)
  {
    auto i0 = reinterpret_cast<const uint8_t*>(str.c_str());
    return AddBytes(i0, i0 + str.size());
  }

  Hash& AddBytes(const uint8_t* i0, const uint8_t* i1)
  {
    while (i0 != i1)
    {
      mValue = Concatenate(*i0);
      ++i0;
    }
    return *this;
  }

  template <typename T>
  Hash& AddObjectBytes(const T& value)
  {
    auto i0 = reinterpret_cast<const uint8_t*>(&value);
    auto i1 = i0 + sizeof(T);
    while (i0 != i1)
    {
      mValue = Concatenate(*i0);
      ++i0;
    }
    return *this;
  }

  operator uint64_t() const
  {
    return mValue;
  }

private:
  uint64_t mValue;

  uint64_t Concatenate(uint64_t value)
  {
    return mValue * 31 + value;
  }
};

}

#endif // LIBDLI_HASH_H_
