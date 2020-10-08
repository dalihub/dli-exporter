#ifndef LIBDLI_INDEX_H_
#define LIBDLI_INDEX_H_
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
#include <cstdint>

namespace dli
{

using Index = uint32_t;

enum LIBDLI_API : Index { INVALID_INDEX = static_cast<Index>(-1) };

}

#endif //LIBDLI_INDEX_H_
