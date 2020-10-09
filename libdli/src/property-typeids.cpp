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
#include "libdli/utils.h"
#include "libdli/property-typeids.h"

using namespace Dali;

namespace dli
{
namespace
{
constexpr uint32_t PROPERTY_TYPE_IDS[] = {
  FourCC("#ERR"), //NONE, can't have that
  FourCC("bool"), //BOOLEAN
  FourCC("flot"), //FLOAT
  FourCC("intg"), //INTEGER
  FourCC("vec2"), //VECTOR2
  FourCC("vec3"), //VECTOR3
  FourCC("vec4"), //VECTOR4
  FourCC("mat3"), //MATRIX3
  FourCC("mat4"), //MATRIX
  FourCC("rect"), //RECTANGLE
  FourCC("quat"), //ROTATION
  FourCC("#ERR"), //STRING - not supported
  FourCC("#ERR"), //ARRAY - not supported
  FourCC("#ERR"), //MAP - not supported
  FourCC("exts"), //EXTENTS
};
}

uint32_t GetPropertyTypeId(Property::Type type)
{
  return PROPERTY_TYPE_IDS[type];
}

std::string GetPropertyTypeIdString(Property::Type type)
{
  return std::string(reinterpret_cast<const char*>(PROPERTY_TYPE_IDS + type), 4);
}

Dali::Property::Type DecodePropertyTypeId(uint32_t typeId)
{
  auto iEnd = PROPERTY_TYPE_IDS + std::extent<decltype(PROPERTY_TYPE_IDS)>::value;
  auto iFind = std::find(PROPERTY_TYPE_IDS, iEnd, typeId);
  return iFind != iEnd ?
    static_cast<Dali::Property::Type>(iFind - PROPERTY_TYPE_IDS) : Property::Type::NONE;
}

}
