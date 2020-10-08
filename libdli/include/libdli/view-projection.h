#ifndef LIBDLI_VIEW_PROJECTION_H_
#define LIBDLI_VIEW_PROJECTION_H_
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

namespace dli
{

/**
 * @brief Contains view and projection matrices, also caching the view-projection
 *      and inverse projection matrices.
 */
class ViewProjection
{
public:
  Dali::Matrix& GetView() { return mView; }
  Dali::Matrix& GetProjection() { return mProjection; }

  ///@brief Updates the cached view projection and inverse projection matrices.
  void Update();

  const Dali::Matrix& GetView() const { return mView; }
  const Dali::Matrix& GetProjection() const { return mProjection; }
  const Dali::Matrix& GetInverseProjection() const { return mInvProjection; }
  const Dali::Matrix& GetViewProjection() const { return mViewProjection; }

private:
  Dali::Matrix mView;
  Dali::Matrix mProjection;
  Dali::Matrix mInvProjection;
  Dali::Matrix mViewProjection;
};

}

#endif //LIBDLI_VIEW_PROJECTION_H_
