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
#include "libdli/string-view.h"
#include <algorithm>
#include <cstring>

namespace dli
{

StringView::StringView(const char* cs)
:  StringView(cs, strlen(cs))
{}

StringView::StringView(const char* cs, size_t size)
:  mString(cs),
  mSize(size)
{}

int StringView::Compare(const char* cs, size_t sSize) const
{
  auto shorter = std::min(mSize, sSize);
  auto base = strncmp(mString, cs, shorter);
  return ((base != 0) || (sSize == mSize)) ? base :
    ((mSize < sSize) ? -cs[shorter]  : mString[shorter]);
}

int StringView::Compare(const char* cs) const
{
  return Compare(cs, strlen(cs));
}

int StringView::Compare(const std::string& s) const
{
  return Compare(s.c_str(), s.size());
}

int StringView::Compare(const StringView & sv) const
{
  return Compare(sv.mString, sv.mSize);
}

}
