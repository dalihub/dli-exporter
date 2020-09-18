#ifndef LIBDLI_MESH_GEOMETRY_H
#define LIBDLI_MESH_GEOMETRY_H
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

// EXTERNAL
#include "dali/public-api/rendering/geometry.h"
#include "dali/public-api/rendering/texture.h"

namespace dli
{

struct MeshGeometry
{
  Dali::Geometry geometry;                         ///< The array of vertices.
  Dali::Texture blendShapeGeometry;                ///< The array of vertices of the different blend shapes encoded inside a texture with power of two dimensions.
  Dali::Vector<float> blendShapeUnnormalizeFactor; ///< Factor used to unnormalize the geometry of the blend shape.
  unsigned int blendShapeBufferOffset;             ///< Offset used to calculate the start of each blend shape.
};

} // namespace dli

#endif // LIBDLI_MESH_GEOMETRY_H
