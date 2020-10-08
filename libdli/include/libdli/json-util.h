#ifndef LIBDLI_JSON_UTIL_H_
#define LIBDLI_JSON_UTIL_H_
/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include "dali/public-api/common/vector-wrapper.h"
#include "dali/public-api/animation/time-period.h"
#include "dali/public-api/object/property.h"
#include "dali/public-api/math/vector4.h"
#include "dali-toolkit/devel-api/builder/tree-node.h"
#include "index.h"

namespace dli
{

LIBDLI_API bool ReadBool(const Dali::Toolkit::TreeNode* node, bool& num);

LIBDLI_API bool ReadInt(const Dali::Toolkit::TreeNode* node, int& num);

LIBDLI_API bool ReadFloat(const Dali::Toolkit::TreeNode* node, float& num);

LIBDLI_API bool ReadIndex(const Dali::Toolkit::TreeNode* node, Index& num);

LIBDLI_API bool ReadBlob(const Dali::Toolkit::TreeNode* node, unsigned int& offset, unsigned int& length);

///@brief Gets the number of numerical element of a JSON array;
///@return 0 if not an array, otherwise the number of float or integer elements
/// at the front of the array.
LIBDLI_API size_t GetNumericalArraySize(const Dali::Toolkit::TreeNode* node);

LIBDLI_API bool ReadVector(const Dali::Toolkit::TreeNode* node, float* num, unsigned int size);

LIBDLI_API bool ReadVector(const Dali::Toolkit::TreeNode* node, int* num, unsigned int size);

///@brief Reads a color.
///
///The node contents could be a vector of 4 floats [r,g,b,a].
///
///@param[in] node The tree node with the color.
///@param[out] color The RGBA color.
///
///@return true if succedded to read the color.
LIBDLI_API bool ReadColor(const Dali::Toolkit::TreeNode* node, Dali::Vector4& color);

LIBDLI_API bool ReadTimePeriod(const Dali::Toolkit::TreeNode* node, Dali::TimePeriod& timePeriod);

LIBDLI_API bool ReadString(const Dali::Toolkit::TreeNode* node, std::string& strValue);

LIBDLI_API bool ReadStringVector(const Dali::Toolkit::TreeNode* node, std::vector<std::string>& strvector);

///@brief Attempts to read a property of the given type from the given JSON node.
///@return The property value that it could interpret. If unsuccessful, its type will be NONE.
///@note Currently only numerical types are supported (including boolean).
LIBDLI_API Dali::Property::Value ReadPropertyValue(const Dali::Property::Type& propType, const Dali::Toolkit::TreeNode& tn);

///@brief Attempts to read a property, whose type it will attempt to determine from the given
/// JSON node.
///@return The property value that it could interpret. If unsuccessful, its type will be NONE.
///@note Currently only numerical types are supported (including boolean).
///@note Supports a disambiguation syntax, whereby the type can be specified explicitly:
/// { "type": "rotation", "value": [...] } .
///@note: rotation / rectangle / extents type properties must be disambiguated in all circumstances.
LIBDLI_API Dali::Property::Value ReadPropertyValue(const Dali::Toolkit::TreeNode& tn);

}

#endif //LIBDLI_JSON_UTIL_H_
