#ifndef LIBDLI_GLTF2_LOADER_H
#define LIBDLI_GLTF2_LOADER_H
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
#include <string>

namespace gltf2
{
struct Camera;
} // namespace gltf2

namespace dli
{
struct CameraParameters;
struct LoadResult;
class ShaderDefinitionFactory;

///@brief Loads the scene from the glTF file located at @a url, storing the results in @a params.
///@note Will throw std::runtime_error for JSON entities with types mismatching expectations, carrying
/// invalid values, or I/O errors.
LIBDLI_API void LoadGltfScene(const std::string& url, ShaderDefinitionFactory& shaderFactory, LoadResult& params);

///@brief Utility function to convert a gltf camera to our runtime representation.
LIBDLI_API void ConvertCamera(const gltf2::Camera& gltfCamera, CameraParameters& result);

} // namespace dli

#endif //LIBDLI_GLTF2_LOADER_H
