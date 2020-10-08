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
#include "libdli/environment-definition.h"
#include "libdli/utils.h"

using namespace Dali;

namespace dli
{

EnvironmentDefinition::RawData
  EnvironmentDefinition::LoadRaw(const std::string& environmentsPath) const
{
  RawData raw;
  auto loadFn = [&environmentsPath](const std::string& path, CubeData& cd) {
    if (path.empty())
    {
      cd.data.resize(6);
      for (auto& face : cd.data)
      {
        face.push_back(PixelData::New(new uint8_t[3]{ 0xff, 0xff, 0xff }, 3, 1, 1, Pixel::RGB888, PixelData::DELETE_ARRAY));
      }
    }
    else if(!LoadCubeMapData(environmentsPath + path, cd))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to load cubemap texture from '" <<
        path << "'.";
    }
  };

  loadFn(mDiffuseMapPath, raw.mDiffuse);
  loadFn(mSpecularMapPath, raw.mSpecular);
  return raw;
}

EnvironmentDefinition::Textures EnvironmentDefinition::Load(RawData&& raw) const
{
  Textures textures;

  // This texture should have 6 faces and only one mipmap
  if (!raw.mDiffuse.data.empty())
  {
    textures.mDiffuse = raw.mDiffuse.CreateTexture();
  }

  // This texture should have 6 faces and 6 mipmaps
  if (!raw.mSpecular.data.empty())
  {
    textures.mSpecular = raw.mSpecular.CreateTexture();
  }
  return textures;
}

}
