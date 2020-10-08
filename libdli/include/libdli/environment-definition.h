#ifndef LIBDLI_ENVIRONMENT_DEFINITION_H
#define LIBDLI_ENVIRONMENT_DEFINITION_H
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
#include "ktx-loader.h"

// EXTERNAL INCLUDES
#include "dali/public-api/rendering/texture.h"
#include "dali/public-api/math/quaternion.h"

namespace dli
{

///@brief Defines an environment map with either or both of radiance
/// and irradiance maps.
struct EnvironmentDefinition
{
  struct Textures
  {
    Dali::Texture mDiffuse;  // irradiance
    Dali::Texture mSpecular;  // radiance

    bool IsLoaded() const
    {
      return mDiffuse || mSpecular;
    }
  };

  struct RawData
  {
    CubeData mDiffuse;
    CubeData mSpecular;
  };

  using EnvironmentData = std::pair<EnvironmentDefinition, Textures>;
  using Vector = std::vector<EnvironmentData>;

  EnvironmentDefinition() = default;

  EnvironmentDefinition(const EnvironmentDefinition&) = delete;
  EnvironmentDefinition& operator=(const EnvironmentDefinition&) = delete;

  EnvironmentDefinition(EnvironmentDefinition&&) = default;
  EnvironmentDefinition& operator=(EnvironmentDefinition&&) = default;

  ///@brief Loads raw pixel data for the given diffuse and specular maps.
  ///@note This can be done on any thread.
  RawData LoadRaw(const std::string& environmentsPath) const;

  ///@brief Creates DALi cubemap Textures from the pixel data in @a raw, then
  /// returns them in a Textures object.
  ///@note This must only be called from the event thread.
  Textures Load(RawData&& raw) const;

public: // DATA
  std::string mDiffuseMapPath;
  std::string mSpecularMapPath;
  Dali::Quaternion mCubeOrientation = Dali::Quaternion::IDENTITY;
  float mIblIntensity = 1.0f;
};

}

#endif //LIBDLI_ENVIRONMENT_DEFINITION_H
