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
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <iostream>
#include <fstream>

#include "Scene3D.h"
#include "LoadScene.h"
#include "SaveScene.h"

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "Missing input parameter." << std::endl;
    return 1;
  }

  std::string inPath = argv[1];
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile(inPath, 0u);
  if (!scene)
  {
    std::cerr << "Failed to process scene file '" << inPath << "':" << importer.GetErrorString() << std::endl;
    return 1;
  }

  scene = importer.ApplyPostProcessing(aiProcess_CalcTangentSpace | aiProcess_SortByPType | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

  std::string outPath;
  if (argc > 2)
  {
    outPath = argv[2];
  }
  else
  {
    outPath = inPath;
  }

  outPath = outPath.substr(0, inPath.rfind('.'));

  Scene3D scene_data;
  MeshIds meshIds;
  GetSceneNodes(scene_data, meshIds, nullptr, scene, scene->mRootNode);
  PackSceneNodeMeshIds(scene_data, meshIds);
  GetSceneMeshes(scene_data, meshIds, scene);
  GetSceneCameras(scene_data, scene);
  GetSceneLights(scene_data, scene);
  GetAnimations(scene_data, scene);

  std::string outBin = outPath + ".bin";
  std::ofstream ofsBin(outBin, ios::binary);
  std::ofstream ofsDli(outPath + ".dli");

  int result = 0;
  if (!ConvertScene(&scene_data, outBin, ofsDli, ofsBin, false))
  {
    result = 1;
  }
  return result;
}
