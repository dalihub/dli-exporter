#ifndef LIBDLI_ALPHA_FUNCTION_HELPER_H_
#define LIBDLI_ALPHA_FUNCTION_HELPER_H_
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

#include "libdli-api.h"

#include "dali/public-api/animation/alpha-function.h"
#include <string>

namespace dli
{

///@return Given a name, provide a Dali::AlphaFunction; if the name was not
/// recognised, get the default one.
Dali::AlphaFunction LIBDLI_API GetAlphaFunction(const std::string& name, bool* found = nullptr);

///@brief Registers an alpha function only if one with the same @a name has
/// not yet been registered. Throws Dali::Exception the name isn't unique.
void LIBDLI_API RegisterAlphaFunction(const std::string& name, Dali::AlphaFunction alphaFn) noexcept(false);

}

#endif
