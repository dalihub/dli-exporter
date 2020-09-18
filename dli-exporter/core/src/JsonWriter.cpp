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
#include "JsonWriter.h"

JsonWriter::JsonWriter(std::ostream & stream, const char* indentation)
: mStream(stream),
  mIndentation(indentation ? indentation : "")
{}

void JsonWriter::WriteObject(const char* name, bool oneLiner)
{
  WritePreamble(name);
  if (!mScopes.empty())
  {
    mScopes.back().needsComma = true;
  }

  mScopes.push_back({ Scope::OBJECT, oneLiner, false });
  mStream << '{';
  NewLine(oneLiner);
}

void JsonWriter::WriteArray(const char* name, bool oneLiner)
{
  WritePreamble(name);
  if (!mScopes.empty())
  {
    mScopes.back().needsComma = true;
  }

  mScopes.push_back({ Scope::ARRAY, oneLiner, false });
  mStream << '[';
  NewLine(oneLiner);
}

void JsonWriter::WriteValue(const char* name, const char* value)
{
  WritePreamble(name);
  mScopes.back().needsComma = true;
  if (value)
  {
    mStream << "\"" << value << "\"";
  }
  else
  {
    mStream << "null";
  }
}

void JsonWriter::WriteValue(const char* name, int value)
{
  WritePreamble(name);
  mScopes.back().needsComma = true;
  mStream << value;
}

void JsonWriter::WriteValue(const char* name, unsigned int value)
{
  WritePreamble(name);
  mScopes.back().needsComma = true;
  mStream << value;
}

void JsonWriter::WriteValue(const char* name, double value)
{
  WritePreamble(name);
  mScopes.back().needsComma = true;
  mStream << value;
}

void JsonWriter::WriteValue(const char* name, bool value)
{
  WritePreamble(name);
  mScopes.back().needsComma = true;
  mStream << std::boolalpha << value << std::noboolalpha;
}

void JsonWriter::CloseScope()
{
  const Scope scope = mScopes.back();
  mScopes.pop_back();

  if (scope.needsComma)
  {
    NewLine(scope.isOneLiner);
  }
  Indent(scope.isOneLiner);
  mStream.put(scope.type);
}

void JsonWriter::Comma()
{
  if (!mScopes.empty() && mScopes.back().needsComma)
  {
    mStream.put(',');
    NewLine(mScopes.back().isOneLiner);
  }
}

void JsonWriter::Indent(bool oneLiner)
{
  if (!oneLiner && !mIndentation.empty())
  {
    for (auto& i : mScopes)
    {
      mStream << mIndentation;
    }
  }
}

void JsonWriter::Name(const char* name)
{
  assert((name != nullptr && !mScopes.empty() && mScopes.back().type == Scope::OBJECT) ||
    (name == nullptr && (mScopes.empty() || mScopes.back().type == Scope::ARRAY)));
  if (name)
  {
    mStream << "\"" << name << "\": ";
  }
}

void JsonWriter::NewLine(bool oneLiner)
{
  mStream.put(oneLiner ? ' ' : '\n');
}
