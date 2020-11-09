#ifndef LOADSCENE_H
#define LOADSCENE_H
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
#include <vector>

using MeshIds = std::vector<unsigned int>;

///@brief Gets nodes from the aiScene and adds them to @a scene_data, except for
/// camera nodes, which must have no children and no meshes, and match the name
/// of a camera in aiScene.
/// Dli nodes may have no more than 1 mesh; out of any aiNode's meshes, the
/// first one is registered to the Dli node, then separate nodes are created with
/// the rest of the meshes, and the same local transform, but with no name.
/// The meshes are stored as indices into the mesh array of the aiScene. The
/// indices of those actually used are written into the @a meshIds vector.
void GetSceneNodes(Scene3D &scene_data, MeshIds& meshIds, Node3D *parent, const aiScene *scene, const aiNode *aNode);

///@brief Meshes are registered on dli Node3Ds as indices into the mesh array
/// of the aiScene. This function converts them into indices into @a meshIds,
/// as these are the ones that are ultimately written.
void PackSceneNodeMeshIds(Scene3D& scene_data, const MeshIds& meshIds);

///@brief Gets the meshes whose indices are recorded into @a meshIds, from
/// the aiScene and creates Mesh entries into @a scene_data.
void GetSceneMeshes(Scene3D& scene_data, const MeshIds& meshIds, const aiScene *scene);

void GetSceneCameras( Scene3D &scene_data, const aiScene *scene );
void GetSceneLights( Scene3D& scene_data, const aiScene* scene );
void GetAnimations( Scene3D &scene_data, const aiScene *scene );

#endif // LOADSCENE_H
