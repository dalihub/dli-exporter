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

#include "LoadScene.h"
#include "Mesh.h"
#include "Util.h"

#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/pbrmaterial.h"

#include <iostream>
#include <math.h>
#include <functional>
#include <algorithm>
#include <cassert>
#include <sstream>

using namespace std;

namespace
{

const unsigned int MAX_WEIGHTS_PER_VERTEX = std::extent<decltype(IVector4::data)>::value;

bool NodeIsCamera(const aiScene *scene, std::string eName)
{

    for (unsigned int n = 0; n < scene->mNumCameras; n++)
    {
        const aiCamera *acam = scene->mCameras[n];
        string camname;
        camname.assign(acam->mName.data, acam->mName.length);
        if (eName == camname)
            return true;
    }
    return false;
}

void SetNodeMeshAndUpdateIds(const aiScene *scene, unsigned int id, Node3D& node, MeshIds& meshIds)
{
    node.m_MeshId = id;
    node.m_MaterialIdx = scene->mMeshes[id]->mMaterialIndex;
    aiString val;
    scene->mMaterials[node.m_MaterialIdx]->Get(AI_MATKEY_GLTF_ALPHAMODE, val);
    if (!strcmp(val.data, "BLEND"))
    {
        node.m_isBlendEnabled = true;
    }
    auto iInsert = std::lower_bound(meshIds.begin(), meshIds.end(), id);
    if (iInsert == meshIds.end() || *iInsert != id)
    {
        meshIds.insert(iInsert, id);
    }
}

void ConsolidateSkeletons(std::vector<Node3D*> &skeletonRoots)
{
    // Nodes in skeleton roots, who are found to have a parent (that isn't themselves)
    // also in skeleton roots, are removed.
    for (auto i0 = skeletonRoots.begin(), i1 = skeletonRoots.end(); i0 != i1;)
    {
        auto n = *i0;
        n = n->m_Parent;
        bool remove = false;
        while (n)
        {
            auto iFind = std::lower_bound(skeletonRoots.begin(), skeletonRoots.end(), n);
            if (iFind != skeletonRoots.end() && *iFind == n)
            {
                remove = true;
                break;
            }
            n = n->m_Parent;
        }

        if (remove)
        {
            i0 = skeletonRoots.erase(i0);
            i1 = skeletonRoots.end();   // TODO: working with an iterator obtained from std::remove might be more elegant (if it works).
        }
        else
        {
            ++i0;
        }
    }

    // Take each remaining node, and update their children's skeleton reference to them.
    for (auto& n : skeletonRoots)
    {
        assert(n->m_Skeleton == n);
        n->Visit([n](Node3D& node) {
            node.m_Skeleton = n;
        });
    }
}

void ConvertSceneBasedIndicesToSkeletonBased(Scene3D& scene_data)
{
    // The bone indices in our mesh data currently refer to nodes by their index in the scene;
    // We are converting "models", which are half-understood to be structurally immutable
    // - are composed of nodes and (sub)meshes, but nothing will be added or removed -, but
    // intend to use them as "scenes", meaning that new nodes / meshes can be created and removed,
    // which result in the shifting of indices.
    // Now in order to make skinning information more resilient to such changes, we
    // convert the indices (of joints, in the attribute buffers of skinned meshes) to be
    // based on their skeleton rather than the scene.
    // The index of the skeleton root is 0; the rest of the indices are assigned depth-first
    // (see Node3D::GetJoints()).
    for (unsigned int i = 0; i < scene_data.GetNumMeshes(); ++i)
    {
        Mesh* m = scene_data.GetMesh(i);
        if (m->IsSkinned())
        {
            // TODO: parallellize this.
            // First update the skeleton of the mesh.
            auto skeletonRoot = m->m_Skeleton->m_Skeleton;
            assert(skeletonRoot == skeletonRoot->m_Skeleton);    // after consolidation, all bone nodes should point to their root.
            if (m->m_Skeleton != skeletonRoot)
            {
                m->m_Skeleton = skeletonRoot;
            }

            // Now convert the node indices from scene based to skeleton based.
            auto joints = m->m_Skeleton->GetJoints();
            std::vector<unsigned int> jointIds(scene_data.GetNumNodes(), -1); // TODO: this could really be per skeleton.
            for (unsigned int i = 0; i < joints.size(); ++i)
            {
                jointIds[joints[i]->m_Index] = i;
            }

            for (auto &j : m->m_Joints0)
            {
                for (auto& ij : j.data)
                {
                    if (ij == -1)
                    {
                        break;
                    }
                    ij = static_cast<float>(jointIds[static_cast<uint32_t>(ij)]);
                }
            }
        }
    }
}

aiNode* FindLeafNamed(const string& eName, aiNode* node)
{
    if (node->mNumMeshes == 0 &&
        node->mNumChildren == 0 &&
        node->mName.length == eName.size() &&
        strncmp(node->mName.data, eName.c_str(), node->mName.length) == 0)
    {
        return node;
    }
    else
    {
        aiNode* result = nullptr;
        auto iEnd = node->mChildren + node->mNumChildren;
        for (auto i = node->mChildren; i != iEnd; ++i)
        {
            if (auto n = FindLeafNamed(eName, *i))
            {
                result = n;
                break;
            }
        }
        return result;
    }
}

void SetBlendShapeHeader(Mesh* pmesh, unsigned int totalTextureSize)
{
  // Calculates the width and height needed to store the blend shapes into a texture.
  unsigned int pow2 = 0u;
  ++totalTextureSize;
  while (totalTextureSize > 0u)
  {
    ++pow2;
    totalTextureSize = (totalTextureSize >> 1u);
  }

  const unsigned int powWidth = pow2 / 2u;
  const unsigned int powHeight = pow2 - powWidth;

  pmesh->m_BlendShapeHeader.width = 1u << powWidth;
  pmesh->m_BlendShapeHeader.height = 1u << powHeight;
}


} // namespace

void GetSceneNodes(Scene3D &scene_data, MeshIds& meshIds, Node3D *parent, const aiScene *scene, const aiNode *aNode)
{
    if(0 == aNode->mNumMeshes && 0 == aNode->mNumChildren)
    {
        string camname(aNode->mName.data, aNode->mName.length);
        if( NodeIsCamera( scene, camname ) )
        {
            return;
        }
    }

    Node3D *pnode = new Node3D(parent);
    pnode->m_Name.assign(aNode->mName.data,aNode->mName.length);
    Matrix::SetMatrix(aNode->mTransformation, pnode->m_Matrix.data);
    scene_data.AddNode(pnode);

    if (aNode->mNumMeshes > 0)
    {
        auto meshId = aNode->mMeshes[0];
        SetNodeMeshAndUpdateIds(scene, meshId, *pnode, meshIds);

        // Create an anonymous node each for the rest of the meshes, with the same transform.
        for (unsigned int i = 1; i < aNode->mNumMeshes; ++i)
        {
            Node3D* node = new Node3D(parent);
            node->m_Name.assign(aNode->mName.data, aNode->mName.length);
            node->m_Name += "_" + std::to_string(i);

            Matrix::SetMatrix(aNode->mTransformation, node->m_Matrix.data);

            SetNodeMeshAndUpdateIds(scene, aNode->mMeshes[i], *node, meshIds);

            scene_data.AddNode(node);
        }
    }

    for(unsigned int c = 0; c < aNode->mNumChildren; c++ )
    {
        GetSceneNodes(scene_data, meshIds, pnode, scene, aNode->mChildren[c]);
    }
}

void PackSceneNodeMeshIds(Scene3D& scene_data, const MeshIds& meshIds)
{
    for (unsigned int i = 0; i < scene_data.GetNumNodes(); ++i)
    {
        auto node = scene_data.GetNode(i);
        if (node->HasMesh())
        {
            auto iFind = std::lower_bound(meshIds.begin(), meshIds.end(), node->m_MeshId);
            assert(iFind != meshIds.end());
            node->m_MeshId = std::distance(meshIds.begin(), iFind);
        }
    }
}

void GetSceneMeshes(Scene3D& scene_data, const MeshIds& meshIds, const aiScene* scene)
{
    std::vector<Node3D*> skeletonRoots;

    for (auto& i : meshIds)
    {
        struct aiMesh *mesh = scene->mMeshes[i];

        Mesh* pmesh = new Mesh();
        pmesh->m_Indices.reserve(mesh->mNumFaces * 3);
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            struct aiFace *face = &mesh->mFaces[f];

            assert(face->mNumIndices == 3);    // aiProcess_Triangulate
            for (unsigned int i = 0; i < face->mNumIndices; i++)
            {
                unsigned short index = face->mIndices[i];
                pmesh->m_Indices.push_back(index);
            }
        }

        if (mesh->HasPositions())
        {
            pmesh->m_Positions.assign(reinterpret_cast<Vector3*>(mesh->mVertices), reinterpret_cast<Vector3*>(mesh->mVertices + mesh->mNumVertices));
        }
        if (mesh->HasNormals())
        {
            pmesh->m_Normals.assign(reinterpret_cast<Vector3*>(mesh->mNormals), reinterpret_cast<Vector3*>(mesh->mNormals + mesh->mNumVertices));
        }
        if (!mesh->HasTextureCoords(0))
        {
            cout << "Generating default texture coordinates." << endl;
            Vector2 uv;
            pmesh->m_Textures.resize(mesh->mNumVertices, uv);
        }
        else
        {
            if (mesh->mNumUVComponents[0] == 2)
            {
                pmesh->m_Textures.reserve(mesh->mNumVertices);
                for (unsigned int co = 0; co < mesh->mNumVertices; co++)
                {
                    Vector2 uv;
                    uv.x = mesh->mTextureCoords[0][co].x;
                    uv.y = mesh->mTextureCoords[0][co].y;
                    pmesh->m_Textures.push_back(uv);
                }
            }
            else
            {
                cout << "3D textures coords not supported." << endl;
            }
        }

        if (!mesh->mTangents)
        {
            cout << "Generating default tangents." << endl;
            Vector3 AXIS_Z;
            AXIS_Z.z = 1.f;
            pmesh->m_Tangents.resize(mesh->mNumVertices, AXIS_Z);
        }
        else
        {
            pmesh->m_Tangents.assign(reinterpret_cast<Vector3*>(mesh->mTangents), reinterpret_cast<Vector3*>(mesh->mTangents + mesh->mNumVertices));
        }

        if (0 != mesh->mNumBones)   // Get skinning data
        {
            pmesh->m_Joints0.resize(pmesh->m_Positions.size(), Vector4 { .0, .0, .0, .0 });
            pmesh->m_Weights0.resize(pmesh->m_Positions.size(), Vector4 { .0, .0, .0, .0 });

            std::vector<int> nextBone;
            nextBone.resize(pmesh->m_Positions.size(), 0);

            auto iBone = mesh->mBones;
            for (auto endBones = iBone + mesh->mNumBones; iBone != endBones; ++iBone)
            {
                auto bone = *iBone;
                if (bone->mNumWeights == 0)
                {
                    continue;
                }

                auto boneNode = scene_data.FindNodeNamed(bone->mName.C_Str());
                if (!boneNode)
                {
                    cout << "ERROR: Bone '" << bone->mName.C_Str() << "' of mesh '" << mesh->mName.C_Str() <<
                        "' references invalid joint '" << bone->mName.C_Str() << "'." << endl;
                    return;
                }

                if (!boneNode->m_InverseBindPoseMatrix)
                {
                    boneNode->m_InverseBindPoseMatrix.reset(new Matrix(bone->mOffsetMatrix));
                }

                // register the new skeleton.
                auto iInsert = std::lower_bound(skeletonRoots.begin(), skeletonRoots.end(), boneNode);
                if (iInsert == skeletonRoots.end() || *iInsert != boneNode)
                {
                    skeletonRoots.insert(iInsert, boneNode);
                    boneNode->m_Skeleton = boneNode;
                }

                if (!pmesh->m_Skeleton)
                {
                    // Assign the first bone as the skeleton of the mesh. At this point we don't know the
                    // relation of the bone nodes, so this is potentially not the root of the skeleton.
                    // However, after we have consolidated the skeletons, we will know the root - we update
                    // the skeleton of the mesh to it at that point.
                    pmesh->m_Skeleton = boneNode->m_Skeleton;
                }

                // save weights
                auto iWeight = bone->mWeights;
                for (auto endWeights = iWeight + bone->mNumWeights; iWeight != endWeights; ++iWeight)
                {
                    if (iWeight->mWeight == 0.f)
                    {
                        continue;
                    }
                    int iNextBone = nextBone[iWeight->mVertexId];
                    if (iNextBone < MAX_WEIGHTS_PER_VERTEX)
                    {
                        // NOTE: at this point we're writing the scene based joint (node) ids;
                        // we will convert it to a skeleton basad index once we've established
                        // the skeletons.
                        pmesh->m_Joints0[iWeight->mVertexId].data[iNextBone] = static_cast<float>(boneNode->m_Index);
                        pmesh->m_Weights0[iWeight->mVertexId].data[iNextBone] = iWeight->mWeight;
                        ++nextBone[iWeight->mVertexId];
                    }
                    else
                    {
                        cout << "WARNING: Vertex " << iWeight->mVertexId << " of mesh '" << mesh->mName.C_Str() <<
                            "' exceeds the number of supported weights." << endl;
                    }
                }
            }
        }

    // Read the blend shapes
    if ((0u != mesh->mNumAnimMeshes) && (nullptr != mesh->mAnimMeshes))
    {
      pmesh->m_MorphMethod = mesh->mMethod;

      pmesh->m_BlendShapes.resize(mesh->mNumAnimMeshes);
      unsigned int totalTextureSize = 0u;
      auto index = 0u;
      for (auto meshIt = mesh->mAnimMeshes; index < mesh->mNumAnimMeshes; ++meshIt, ++index)
      {
        auto animMesh = *meshIt;

        BlendShape& blendShape = pmesh->m_BlendShapes[index];

        blendShape.m_Name = std::string( animMesh->mName.data, animMesh->mName.length );

        if (animMesh->HasPositions())
        {
          blendShape.m_Positions.assign(reinterpret_cast<Vector3*>(animMesh->mVertices), reinterpret_cast<Vector3*>(animMesh->mVertices + animMesh->mNumVertices));
          totalTextureSize += animMesh->mNumVertices;
        }
        if (animMesh->HasNormals())
        {
          blendShape.m_Normals.assign(reinterpret_cast<Vector3*>(animMesh->mNormals), reinterpret_cast<Vector3*>(animMesh->mNormals + animMesh->mNumVertices));
          totalTextureSize += animMesh->mNumVertices;
        }

        if (animMesh->HasTangentsAndBitangents())
        {
          blendShape.m_Tangents.assign(reinterpret_cast<Vector3*>(animMesh->mTangents), reinterpret_cast<Vector3*>(animMesh->mTangents + animMesh->mNumVertices));
          totalTextureSize += animMesh->mNumVertices;
        }

        blendShape.m_Weight = 0.f;
      }

      SetBlendShapeHeader(pmesh, totalTextureSize);
    }

        scene_data.AddMesh(pmesh);
    }

    ConsolidateSkeletons(skeletonRoots);

    // Add skeletons to scene.
    for (auto i : skeletonRoots)
    {
        scene_data.AddSkeletonRoot(i);
    }

    ConvertSceneBasedIndicesToSkeletonBased(scene_data);
}

aiNode *GetCameraNode( const aiScene *scene, aiNode *aNode )
{
    if(!aNode->mNumMeshes)
    {
        if(!aNode->mNumChildren)
        {
            string camname;
            camname.assign(aNode->mName.data,aNode->mName.length);
            if( NodeIsCamera( scene, camname ) )
            {
                return aNode;
            }
        }
    }

    for(unsigned int c = 0; c < aNode->mNumChildren; c++ )
    {
        aiNode *returnNode = GetCameraNode( scene, aNode->mChildren[c]);
        if(returnNode)
            return returnNode;
    }
    return nullptr;
}

void GetSceneCameras( Scene3D &scene_data, const aiScene *scene )
{
    for( unsigned int n = 0; n < scene->mNumCameras; n++ )
    {
        const aiCamera *acam = scene->mCameras[n];
        Camera3D cam;
        cam.far = acam->mClipPlaneFar;
        cam.near = acam->mClipPlaneNear;
        cam.fov = acam->mHorizontalFOV *180.0f / 3.14159265358979323846f;
        Vector3 vlookat;
        Vector3 vUp;
        Vector3 vPosition;
        vlookat.x = acam->mLookAt.x;
        vlookat.y = acam->mLookAt.y;
        vlookat.z = acam->mLookAt.z;
        vUp.x = acam->mUp.x;
        vUp.y = acam->mUp.y;
        vUp.z = acam->mUp.z;
        vPosition.x = acam->mPosition.x;
        vPosition.y = acam->mPosition.y;
        vPosition.z = acam->mPosition.z;
        aiNode *pnode = GetCameraNode( scene, scene->mRootNode );
        if( pnode )
        {
            aiMatrix4x4 globalMat(pnode->mTransformation );
            while ( pnode = pnode->mParent )
            {
                globalMat = pnode->mTransformation * globalMat;
            }
            cam.SetMatrix( globalMat );
            cam.MultLookAtMatrix( vlookat, vUp , vPosition );
            scene_data.AddCamera( cam );
        }
    }
}

void GetSceneLights( Scene3D& scene_data, const aiScene* scene)
{
    for( unsigned int n = 0; n < scene->mNumLights; ++n)
    {
        const aiLight* aLight = scene->mLights[n];
        string name(aLight->mName.data, aLight->mName.length);

        Light light;
        aiNode* node = FindLeafNamed( name, scene->mRootNode );
        if (node)
        {
            light.SetMatrix(node->mTransformation);
            light.SetDiffuseColor(aLight->mColorDiffuse);
            scene_data.AddLight(light);
        }
    }
}

void GetAnimations( Scene3D &scene_data, const aiScene *scene )
{
    if(!scene->HasAnimations())
    {
        return;
    }

    int autoGeneratedNames = 0;
    for( unsigned int a = 0; a < scene->mNumAnimations; a++)
    {
        aiAnimation *animation = scene->mAnimations[a];
        if((0u == animation->mNumChannels) && (0u == animation->mNumMorphMeshChannels))
        {
            continue;
        }
        Animation3D dataAnim;

        dataAnim.Duration = static_cast<float>(animation->mDuration);
        dataAnim.TicksPerSecond = (fabsf(static_cast<float>(animation->mTicksPerSecond)) < Util::EPSILON) ? 1000.f : static_cast<float>(animation->mTicksPerSecond);

        if (animation->mName.length > 0u)
        {
            dataAnim.Name.assign(animation->mName.data, animation->mName.length);
        }
        else
        {
            ++autoGeneratedNames;
            std::ostringstream buffer;
            buffer << "autogenerated-name#" << autoGeneratedNames;
            dataAnim.Name = buffer.str();
        }

        for( unsigned int ch = 0; ch < animation->mNumChannels; ch++)
        {
            bool diffFlag = false;
            aiQuaternion cquat;
            aiVector3D cvec;
            aiNodeAnim* nAnim = animation->mChannels[ch];
            NodeAnimation3D nodeAnim;
            nodeAnim.NodeName.assign( nAnim->mNodeName.data, nAnim->mNodeName.length );

            for( unsigned int k = 0; k < nAnim->mNumRotationKeys; k++ )
            {
                aiQuatKey qkey = nAnim->mRotationKeys[k];
                aiQuatKey qtrimKey = qkey;
                qtrimKey.mValue.x = Util::trim(qkey.mValue.x);
                qtrimKey.mValue.y = Util::trim(qkey.mValue.y);
                qtrimKey.mValue.z = Util::trim(qkey.mValue.z);
                qtrimKey.mValue.w = Util::trim(qkey.mValue.w);
                NodeKey nkey;
                nkey.time = static_cast<float>(qkey.mTime);
                nkey.v[0] = qkey.mValue.x;
                nkey.v[1] = qkey.mValue.y;
                nkey.v[2] = qkey.mValue.z;
                nkey.v[3] = qkey.mValue.w;
                nodeAnim.Rotations.push_back( nkey );
                if(!k)
                {
                  cquat = qtrimKey.mValue;
                }
                else if(cquat != qtrimKey.mValue)
                {
                    diffFlag = true;
                }
            }
            if( !diffFlag )
            {
                nodeAnim.Rotations.clear();
            }
            diffFlag = false;
            for( unsigned int k = 0; k < nAnim->mNumPositionKeys; k++ )
            {
                aiVectorKey vkey = nAnim->mPositionKeys[k];
                aiVectorKey trimKey = vkey;
                trimKey.mValue[0] = Util::trim(vkey.mValue[0]);
                trimKey.mValue[1] = Util::trim(vkey.mValue[1]);
                trimKey.mValue[2] = Util::trim(vkey.mValue[2]);
                NodeKey nkey;
                nkey.time = static_cast<float>(vkey.mTime);
                nkey.v[0] = vkey.mValue[0];
                nkey.v[1] = vkey.mValue[1];
                nkey.v[2] = vkey.mValue[2];
                nodeAnim.Positions.push_back( nkey );
                if(!k)
                {
                  cvec = trimKey.mValue;
                }
                else if(cvec != trimKey.mValue)
                {
                    diffFlag = true;
                }
            }
            if( !diffFlag )
            {
                nodeAnim.Positions.clear();
            }
            diffFlag = false;
            for( unsigned int k = 0; k < nAnim->mNumScalingKeys; k++ )
            {
                aiVectorKey vkey = nAnim->mScalingKeys[k];
                aiVectorKey trimKey = vkey;
                trimKey.mValue[0] = Util::trim(vkey.mValue[0]);
                trimKey.mValue[1] = Util::trim(vkey.mValue[1]);
                trimKey.mValue[2] = Util::trim(vkey.mValue[2]);
                NodeKey nkey;
                nkey.time = static_cast<float>(vkey.mTime);
                nkey.v[0] = vkey.mValue[0];
                nkey.v[1] = vkey.mValue[1];
                nkey.v[2] = vkey.mValue[2];
                nodeAnim.Scales.push_back( nkey );
                if(!k)
                {
                  cvec = trimKey.mValue;
                }
                else if(cvec != trimKey.mValue)
                {
                    diffFlag = true;
                }
            }
            if( !diffFlag )
            {
                nodeAnim.Scales.clear();
            }

            if( !nodeAnim.Rotations.empty() || !nodeAnim.Positions.empty() || !nodeAnim.Scales.empty() )
            {
                dataAnim.AnimNodesList.push_back(nodeAnim);
            }
        }

        for (unsigned int ch = 0; ch < animation->mNumMorphMeshChannels; ++ch)
        {
            const aiMeshMorphAnim* const nAnim = animation->mMorphMeshChannels[ch];

            std::vector<NodeAnimation3D> nodeAnims;

            for (unsigned int k = 0; k < nAnim->mNumKeys; k++)
            {
                const aiMeshMorphKey& vkey = nAnim->mKeys[k];

                for (unsigned int w = 0u; w < vkey.mNumValuesAndWeights; ++w)
                {
                    if (w >= nodeAnims.size())
                    {
                        NodeAnimation3D nodeAnim;
                        nodeAnim.NodeName.assign(nAnim->mName.data, nAnim->mName.length);

                        nodeAnims.emplace_back(nodeAnim);
                    }
                    NodeAnimation3D& nodeAnim = nodeAnims[w];

                    NodeKey nkey;
                    nkey.time = static_cast<float>(vkey.mTime);
                    nkey.v[0] = static_cast<float>(vkey.mWeights[w]);
                    nodeAnim.Weights.push_back(nkey);
                }
            }

            for (const auto& nodeAnim : nodeAnims)
            {
                if (!nodeAnim.Weights.empty())
                {
                    dataAnim.AnimNodesList.emplace_back(nodeAnim);
                }
            }

        }

        scene_data.AddAnimation(dataAnim);
    }
}
