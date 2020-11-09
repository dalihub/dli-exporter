#ifndef JSONWRITER_H
#define JSONWRITER_H
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

#include <sstream>
#include <vector>
#include <cassert>

///@brief Facilitates the writing of a JSON document into a given std::ostream.
/// Performs line breaks, indentation, and comma separation automatically.
/// Does not perform additional caching; writes directly to the stream.
///
/// The indentation is set with the string passed to the constructor.
/// Line breaks on objects and arrays can be controlled with the oneLiner parameter
/// of Write{Object|Array}(), i.e.:
/// true: [ 1.0, 2.0 ]
/// false (default): [
///   1.0,
///   2.0,
/// ]
///
/// Usage example:
/// std::fstream myJsonFile("books.json"); // open stream
/// JsonWriter w(myJsonFile, "\t"); // start writing, indent with a tab character (nothing is written yet)
/// w.WriteObject(nullptr);      // write root element -- an object | {
/// w.WriteArray("fiction");        // write array called "fiction" |   "fiction": [
/// w.WriteObject(nullptr, true); // write book object as one liner |     {
/// w.WriteValue("author", "Stephen King");  // writes on same line |       "author": "Stephen King"
/// w.WriteValue("title", "The Stand");      // writes on same line |                               , "title": "The Stand"
/// w.WriteValue("rating", 3.58);           // writes numeric value |                                                     , "rating": 3.58
/// w.CloseScope();                     // closes book item's scope |                                                                      }
/// w.CloseScope();                       // closes "fiction" array |   ]
/// w.WriteArray("cooking");        // write array called "cooking" |    , // on previous line, after array
///                                                              // |   "cooking": [
/// w.CloseScope();                       // closes "cooking" array |   ]
/// w.CloseScope();                          // closes root element | }
/// myJsonFile << std::endl; // write an endl at the end of the file
/// myJsonFile.close(); // finish writing the file
///
class JsonWriter
{
public:
  JsonWriter(std::ostream& stream, const char* indentation);

  ///@brief Opens an object scope.
  ///@param name If the parent scope is an array (or root) this must be null, otherwise
  /// this is the key. NOTE: special characters must be escaped manually.
  ///@param oneLiner Whether line breaks should be inserted between the children of
  /// this new object. NOTE: only affects immediate children.
  void WriteObject(const char* name, bool oneLiner = false);

  ///@brief Opens an array scope.
  ///@param name If the parent scope is an array (or root) this must be null, otherwise
  /// this is the key. NOTE: special characters must be escaped manually.
  ///@param oneLiner Whether line breaks should be inserted between the children of
  /// this new array. NOTE: only affects immediate children.
  void WriteArray(const char* name, bool oneLiner = false);

  ///@brief Writes a string value, automatically enclosing it in quotes.
  ///@param name If the parent scope is an array this must be null, otherwise
  /// this is the key. NOTE: special characters must be escaped manually.
  /// NOTE: pass nullptr to write the JSON 'null' (no quotes).
  ///@param value the value to write. NOTE: special characters must be escaped manually.
  void WriteValue(const char* name, const char* value);

  ///@brief Writes an integer value.
  ///@param name If the parent scope is an array this must be null, otherwise
  /// this is the key. NOTE: special characters must be escaped manually.
  ///@param value the value to write.
  void WriteValue(const char* name, int value);

  ///@brief Writes an unsigned integer value.
  ///@param name If the parent scope is an array this must be null, otherwise
  /// this is the key. NOTE: special characters must be escaped manually.
  ///@param value the value to write.
  void WriteValue(const char* name, unsigned int value);

  ///@brief Writes a double precision value.
  ///@param name If the parent scope is an array this must be null, otherwise
  /// this is the key. NOTE: special characters must be escaped manually.
  ///@param value the value to write.
  void WriteValue(const char* name, double value);

  ///@brief Writes a boolean value as 'true' or 'false' (no quotes).
  ///@param name If the parent scope is an array this must be null, otherwise
  /// this is the key. NOTE: special characters must be escaped manually.
  ///@param value the value to write.
  void WriteValue(const char* name, bool value);

  ///@brief Closes the currently written scope.
  void CloseScope();

private:
  struct Scope
  {
    enum Type
    {
      ARRAY = ']',
      OBJECT = '}',
    };

    Type type;
    bool isOneLiner : 1;
    bool needsComma : 1;
  };

  std::ostream& mStream;
  std::vector<Scope> mScopes;
  std::string mIndentation;

  void Comma();
  void Indent(bool oneLiner);
  void Name(const char* name);
  void NewLine(bool oneLiner);

  void WritePreamble(const char* name)
  {
    Comma();
    Indent(mScopes.empty() ? false : mScopes.back().isOneLiner);
    Name(name);
  }
};

#endif //JSONWRITER_H
