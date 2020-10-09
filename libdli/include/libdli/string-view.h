#ifndef LIBDLI_STRING_VIEW_H_
#define LIBDLI_STRING_VIEW_H_
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
#include <string>

namespace dli
{

///@brief View of a string, typically from the original json buffer. This saves on allocations.
///@note Shouldn't outlive the buffer it was created from, and any lingering instances mustn't be
/// used once that's gone.
struct StringView
{
  StringView() = default;
  StringView(const char* cs);
  StringView(const char* cs, size_t size);

  size_t size() const
  {
    return mSize;
  }

  operator const char*() const
  {
    return mString;
  }

  int Compare(const char* cs, size_t sSize) const;
  int Compare(const char* cs) const;
  int Compare(const std::string& s) const;
  int Compare(const StringView& sv) const;

  std::string ToString() const
  {
    return std::string(mString, mSize);
  }

  bool operator<(const StringView& rhs) const
  {
    return Compare(rhs) < 0;
  }

private:
  const char* mString = nullptr;
  size_t mSize = 0;
};

}

#endif
