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

#include "Scene3D.h"

Scene3D::Scene3D()
{
}

Scene3D::~Scene3D()
{
    for(unsigned int i=0; i < m_nodes.size(); i++)
    {
        delete m_nodes[i];
    }
    m_nodes.clear();
}

void Scene3D::AddNode(Node3D *enode)
{
    enode->m_Index = m_nodes.size();
    m_nodes.push_back(enode);
}

unsigned int Scene3D::GetNumNodes() const
{
    return m_nodes.size();
}

unsigned int Scene3D::GetNumMeshes() const
{
  return m_meshes.size();
}

unsigned int Scene3D::GetNumSkeletonRoots() const
{
  return m_skeletonRoots.size();
}

Node3D *Scene3D::GetNode(unsigned int idx)
{
    return m_nodes[idx];
}

Node3D* Scene3D::FindNodeNamed(const std::string& name) const
{
  auto iFind = std::find_if(m_nodes.begin(), m_nodes.end(), [&name](const Node3D* n){
    return n->m_Name == name;
  });
  return iFind != m_nodes.end() ? *iFind : nullptr;
}

void Scene3D::AddMesh(Mesh* emesh)
{
  m_meshes.push_back(emesh);
}

Mesh* Scene3D::GetMesh(unsigned int idx) const
{
  return m_meshes[idx];
}

void Scene3D::AddSkeletonRoot(Node3D * node)
{
    m_skeletonRoots.push_back(node);
}

Node3D* Scene3D::GetSkeletonRoot(unsigned int idx) const
{
    return m_skeletonRoots[idx];
}

unsigned int Scene3D::FindSkeletonId(const Node3D* skeletonRoot) const
{
    auto iFirst = m_skeletonRoots.begin();
    return std::distance(iFirst, std::find(iFirst, m_skeletonRoots.end(), skeletonRoot));
}

void Scene3D::AddCamera(Camera3D &ecam)
{
    m_cameras.push_back(ecam);
}

void Scene3D::AddLight(const Light& eLight)
{
    m_lights.push_back(eLight);
}

unsigned int Scene3D::GetNumCameras() const
{
    return m_cameras.size();
}

unsigned int Scene3D::GetNumLights() const
{
    return m_lights.size();
}

Camera3D* Scene3D::GetCamera(unsigned int idx)
{
    return &m_cameras[idx];
}

Light* Scene3D::GetLight(unsigned int idx)
{
    return &m_lights[idx];
}

void Scene3D::AddAnimation(Animation3D &eanim)
{
    m_animations.push_back(eanim);
}

unsigned int Scene3D::GetNumAnimations() const
{
    return m_animations.size();
}

Animation3D* Scene3D::GetAnimation(unsigned int idx)
{
    return &m_animations[idx];
}

bool Scene3D::HasAnimations()
{
    bool anim = false;
    for( unsigned int i = 0; i < m_animations.size(); i++ )
    {
        anim |= m_animations[i].HasAnimations();
    }
    return anim;
}
