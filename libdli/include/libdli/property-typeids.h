#ifndef LIBDLI_PROPERTY_TYPEIDS_H_
#define LIBDLI_PROPERTY_TYPEIDS_H_
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
#include "dali/public-api/object/property.h"
#include <string>
#include <cstdint>

namespace dli
{

///@return A fourCC relating to the given Dali::Property::Type.
uint32_t GetPropertyTypeId(Dali::Property::Type type);

///@return A fourCC string relating to the given Dali::Property::Type.
std::string GetPropertyTypeIdString(Dali::Property::Type type);

///@return Property type enum value based on fourCC typeId, NONE if invalid.
Dali::Property::Type DecodePropertyTypeId(uint32_t typeId);

}


#endif //#ifdef LIBDLI_PROPERTY_TYPEIDS_H_
