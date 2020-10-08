#ifndef LIBDLI_SHADER_DEFINITION_FACTORY_H_
#define LIBDLI_SHADER_DEFINITION_FACTORY_H_
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
#include "index.h"

// EXTERNAL INCLUDES
#include "dali/devel-api/common/map-wrapper.h"

namespace dli
{

struct NodeDefinition;
class ResourceBundle;

class LIBDLI_API ShaderDefinitionFactory
{
public:
  ///@brief Input for meshes and materials, output for shaders.
  void SetResources(ResourceBundle& resources);

  ///@brief Produces the index of a shader, which should be used to index into the shaders
  /// vector of the ResourceBundle which was provided for the factory. This shader will be
  /// created if one with the given settings hasn't been created by the factory yet (shaders
  /// already existing in the ResourceBundle are ignored), otherwise the index of the previously
  /// created shader will be returned.
  Index ProduceShader(const NodeDefinition& nodeDef);

private:
  ResourceBundle* mResources;  // no ownership
  std::map<uint64_t, Index> mShaderMap;
};
}

#endif //LIBDLI_SHADER_DEFINITION_FACTORY_H_
