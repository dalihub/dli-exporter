#ifndef LIBDLI_CAMERA_PARAMETERS_H
#define LIBDLI_CAMERA_PARAMETERS_H
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
#include "view-projection.h"

// EXTERNAL INCLUDES
#include "dali/public-api/math/matrix.h"
#include "dali/public-api/math/vector3.h"

namespace dli
{

struct LIBDLI_API CameraParameters
{
  CameraParameters()
  :  matrix(Dali::Matrix::IDENTITY),
    orthographicSize(-1.f, 1.f, 1.f, -1.f),
    yFov(60.f),
    zNear(0.1f),
    zFar(1000.f),
    isPerspective(true)
  {}

  ~CameraParameters()
  {}

  Dali::Matrix matrix;
  Dali::Vector4 orthographicSize;
  float yFov;
  float zNear;
  float zFar;
  bool isPerspective;

  ViewProjection GetViewProjection() const;

  void CalculateTransformComponents(Dali::Vector3& position, Dali::Quaternion& orientation, Dali::Vector3& scale) const;
};

} // namespace dli

#endif // LIBDLI_CAMERA_PARAMETERS_H
