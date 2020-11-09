#ifndef MESH_H
#define MESH_H
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

#include "BlendShapeHeader.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <vector>


struct BlendShape
{
    std::string          m_Name;
    std::vector<Vector3> m_Positions;
    std::vector<Vector3> m_Normals;
    std::vector<Vector3> m_Tangents;
    float m_Weight = 0.f;
};

class Mesh
{
public:
    std::vector<Vector3> m_Positions;
    std::vector<Vector3> m_Normals;
    std::vector<Vector3> m_Tangents;
    std::vector<Vector2> m_Textures;
    std::vector<unsigned short> m_Indices;

    std::vector<Vector4> m_Joints0; // indices into the joints of the skeleton (refer to Node3D::GetJoints()).
    std::vector<Vector4> m_Weights0;

    const Node3D* m_Skeleton = nullptr;

  BlendShapeHeader m_BlendShapeHeader;
    std::vector<BlendShape> m_BlendShapes;
    unsigned int m_MorphMethod = 0u;            ///< The method used for morphing blend shapes. Assimp supports VERTEX_BLEND, MORPH_NORMALIZED and MORPH_RELATIVE.

    bool  IsSkinned() const
    {
      return m_Skeleton != nullptr;
    }
};

#endif //MESH_H
