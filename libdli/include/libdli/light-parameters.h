#ifndef LIBDLI_LIGHT_PARAMETERS_H
#define LIBDLI_LIGHT_PARAMETERS_H
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

// INTERNAL INCLUDES
#include "libdli-api.h"

// EXTERNAL INCLUDES
#include "dali/public-api/math/matrix.h"
#include "dali/public-api/math/vector3.h"
#include <stdint.h>

namespace dli
{

struct LIBDLI_API LightParameters
{
  // NOTE: the position is ignored unless shadow mapping is used.
  Dali::Matrix transform;

  Dali::Vector3 color;
  float intensity;
  float shadowIntensity;
  uint32_t shadowMapSize;
  float orthographicSize;
};

} // namespace dli

#endif // LIBDLI_LIGHT_PARAMETERS_H
