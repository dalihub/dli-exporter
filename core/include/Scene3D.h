#ifndef SCENE3D_H
#define SCENE3D_H
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
#include "Camera3D.h"
#include "Light.h"
#include "Animation3D.h"
#include <vector>

using namespace std;

class Mesh;

class Scene3D
{
    public:
        Scene3D();
        virtual ~Scene3D();
        unsigned int GetNumNodes() const;
        unsigned int GetNumMeshes() const;
        unsigned int GetNumSkeletonRoots() const;
        unsigned int GetNumCameras() const;
        unsigned int GetNumLights() const;
        unsigned int GetNumAnimations() const;
        void AddNode(Node3D *enode);
        Node3D* GetNode(unsigned int idx);
        Node3D* FindNodeNamed(const std::string& name) const;
        void AddMesh(Mesh* emesh);
        Mesh* GetMesh(unsigned int idx) const;
        void AddSkeletonRoot(Node3D* node);
        Node3D* GetSkeletonRoot(unsigned int idx) const;
        unsigned int FindSkeletonId(const Node3D* skeletonRoot) const;
        void AddCamera(Camera3D &ecam);
        Camera3D* GetCamera(unsigned int idx);
        void AddLight(const Light& eLight);
        Light* GetLight(unsigned int idx);
        void AddAnimation(Animation3D &eanim);
        bool HasAnimations();
        Animation3D* GetAnimation(unsigned int idx);
    protected:

    private:
        vector<Node3D*> m_nodes;
        vector<Mesh*> m_meshes;
        vector<Node3D*> m_skeletonRoots;    // no ownership; references m_nodes.
        vector<Camera3D> m_cameras;
        vector<Light> m_lights;
        vector<Animation3D> m_animations;
};

#endif // SCENE3D_H
