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

#include "Node3D.h"
#include "Util.h"
#include <numeric>
#include <regex>

namespace
{
const std::regex NODE_NAME_REGEX = std::regex("[\\s:]");
}

const Node3D::Predicate Node3D::DEFAULT_END_VISIT_PREDICATE = [](const Node3D&) { return false; };

std::string Node3D::MakeValidName(const std::string& name)
{
    return std::regex_replace(name, NODE_NAME_REGEX, "_");
}

Node3D::Node3D(Node3D *eParent)
:   m_Skeleton(nullptr)
{
    m_Parent = eParent;
    if (eParent)
    {
        eParent->m_Children.push_back(this);
    }

    m_Index = -1;
    m_MaterialIdx = 0;
    m_isBlendEnabled = false;
}

Node3D::~Node3D()
{
    //dtor
}

bool Node3D::IsSkeletonRoot() const
{
    return m_Skeleton == this;
}

std::vector<const Node3D*> Node3D::GetJoints() const
{
    std::vector<const Node3D*> joints;
    Visit([&joints](const Node3D& n){
        if (n.m_InverseBindPoseMatrix)
        {
            joints.push_back(&n);
        }
    });
    return joints;
}

bool Node3D::HasMesh() const
{
    return m_MeshId != INVALID_MESH;
}
