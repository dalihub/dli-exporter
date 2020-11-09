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

#include "SaveScene.h"
#include "Mesh.h"
#include "JsonWriter.h"
#include "Util.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>

using namespace std;

namespace
{
using AnimationDataMap = std::map<std::string, std::string>;

const char* const BLEND_SHAPE_VERSION = "2.0";
}

template <typename T>
void WriteArrayData(const T* data, unsigned int n, JsonWriter& writer)
{
  const T* end = data + n;
  while (data != end)
  {
    writer.WriteValue(nullptr, *data);
    ++data;
  }
}

template <unsigned int N, typename T>
void WriteArrayData(const T(&data)[N], JsonWriter& writer)
{
    WriteArrayData(data, N, writer);
}

void SaveNodes(Scene3D *scene, JsonWriter& outDli, bool saveMaterials);
void SaveMeshes(Scene3D *scene, JsonWriter& outDli, ostream &outBin,
    std::string fileNameBinPath);
void SaveCameras(Scene3D *scene, JsonWriter& outDli);
void SaveSkeletons(Scene3D *scene, JsonWriter& outDli);
void SaveLights(Scene3D *scene, JsonWriter& outDli);
void SaveMaterials(Scene3D *scene, JsonWriter& outDli);
void SaveEnvironment(Scene3D *scene, JsonWriter& outDli);
void SaveShaders(Scene3D *scene, JsonWriter& outDli);
void SaveAnimations(Scene3D *scene, JsonWriter& outDli, std::set<std::string>& animNames);
void SaveAnimationsBinary(Scene3D *scene, JsonWriter& outDli, std::string outPath, std::set<std::string>& animNames, AnimationDataMap* animationContents);
void WriteNodeKeyframes(const std::string& property, JsonWriter& outDli,
  Animation3D *animation, int animationidx, const std::vector<NodeKey>& keyframes, unsigned int keyByteSize);
void WriteNodeKeyframesBin(const std::string& url, const std::string& property, JsonWriter& outDli, std::ostream& osBin, unsigned int& offset,
  Animation3D *animation, int animationidx, const std::vector<NodeKey>& keyframes, unsigned int keyByteSize);

void JsonScopeGuard(JsonWriter* w)
{
  w->CloseScope();
}

using JsonWriterScope = std::unique_ptr<JsonWriter, void(*)(JsonWriter*)>;

bool SaveScene(Scene3D *scene, std::string fileNamePath,
    std::string fileNameBinPath, bool saveMaterials)
{
  struct Files
  {
    std::ofstream dliStream;
    std::ofstream binStream;

    Files(std::string dliName, std::string binName)
    {
      dliStream.open(dliName);
      binStream.open(binName, ios::binary);
    }

    ~Files()
    {
      if (dliStream.is_open())
      {
        dliStream.close();
      }

      if (binStream.is_open())
      {
        binStream.close();
      }
    }
  } files(fileNamePath, fileNameBinPath);

  return ConvertScene(scene, fileNameBinPath, files.dliStream, files.binStream, saveMaterials);
}

bool ConvertScene(Scene3D* scene, std::string fileNameBin, std::ostream& outDli,
    std::ostream& outBin, bool saveMaterials, bool binaryAnimations,
    std::map<std::string, std::string>* animationContents)
{
  // If filenameBin is a path, now is a good time to discard all but the filename & extension -
  // the .bin file that we are going to reference must be in the same directory as the .dli.
  auto iDirSeparator = std::min(fileNameBin.rfind('\\'),
      fileNameBin.rfind('/'));
  std::string outDir;
  if (iDirSeparator != std::string::npos)
  {
    outDir = fileNameBin.substr(0, iDirSeparator + 1);
    fileNameBin = fileNameBin.substr(iDirSeparator + 1,
        fileNameBin.length() - iDirSeparator - 1);
  }

  // Write scene data.
  JsonWriter writer(outDli, "  ");
  writer.WriteObject(nullptr);

  writer.WriteObject("asset", true);
    writer.WriteValue("version", "1.0");
  writer.CloseScope();

  writer.WriteValue("scene", 0);

  writer.WriteArray("scenes", true);
    writer.WriteObject(nullptr, true);
      writer.WriteArray("nodes", true);
        writer.WriteValue(nullptr, 0);
      writer.CloseScope();
    writer.CloseScope();
  writer.CloseScope();

  //Save Nodes
  writer.WriteArray("nodes");
  SaveNodes(scene, writer, saveMaterials);
  writer.CloseScope();

  // Save meshes
  writer.WriteArray("meshes");
  SaveMeshes(scene, writer, outBin, fileNameBin);
  writer.CloseScope();

  SaveSkeletons(scene, writer);

  //Save Cameras
  writer.WriteArray("cameras");
  SaveCameras(scene, writer);
  writer.CloseScope();

  //Save Lights
  writer.WriteArray("lights");
  SaveLights(scene, writer);
  writer.CloseScope();

  //Save Materials
  writer.WriteArray("materials");
  SaveMaterials(scene, writer);
  writer.CloseScope();

  //Save Environment
  writer.WriteArray("environment");
  SaveEnvironment(scene, writer);
  writer.CloseScope();

  //Save Shaders
  writer.WriteArray("shaders");
  SaveShaders(scene, writer);
  writer.CloseScope();

  //Save Animations
  if (scene->HasAnimations())
  {
    std::set<std::string> animNames;

    writer.WriteArray("animations");
    if (binaryAnimations)
    {
      SaveAnimationsBinary(scene, writer, outDir, animNames, animationContents);
    }
    else
    {
      SaveAnimations(scene, writer, animNames);
    }
    writer.CloseScope();
  }

  writer.CloseScope();
  outDli << std::endl;
  return true;
}

void SaveNodes(Scene3D *scene, JsonWriter& outDli, bool saveMaterials)
{
  for (unsigned int n = 0; n < scene->GetNumNodes(); n++)
  {
    outDli.WriteObject(nullptr);
    Node3D *node = scene->GetNode(n);
    if (!node->m_Name.empty())
    {
      auto validName = Node3D::MakeValidName(node->m_Name);
      outDli.WriteValue("name", validName.c_str());
    }
    if (!node->m_Matrix.IsIdentity())
    {
      outDli.WriteArray("matrix", true);
      for (int i = 0; i < 16; i++)
      {
        outDli.WriteValue(nullptr, node->m_Matrix.data[i]);
      }
      outDli.CloseScope();
    }

    if (node->HasMesh())
    {
      outDli.WriteObject("model");
      outDli.WriteValue("mesh", node->m_MeshId);
      if (saveMaterials)
      {
        outDli.WriteValue("material", node->m_MaterialIdx + 1);
      }
      outDli.CloseScope();
    }
    if (node->m_Children.size() > 0)
    {
      outDli.WriteArray("children", true);

      for (auto i: node->m_Children)
      {
        outDli.WriteValue(nullptr, i->m_Index);
      }
      outDli.CloseScope();
    }

    if (node->m_InverseBindPoseMatrix)
    {
      outDli.WriteArray("inverseBindPoseMatrix", true);
      for (auto i : node->m_InverseBindPoseMatrix->data)
      {
        outDli.WriteValue(nullptr, i);
      }
      outDli.CloseScope();
    }

    outDli.CloseScope();
  }
}

void WriteBufferInternal(const char* name, unsigned int offset, unsigned int size, JsonWriter& writer)
{
  writer.WriteObject(name, true);
  writer.WriteValue("byteOffset", offset);
  writer.WriteValue("byteLength", size);
  writer.CloseScope();
}

template <typename T>
void WriteBuffer(const char* name, unsigned int offset, unsigned int numElems, JsonWriter& writer, unsigned int& outLength)
{
  outLength = numElems * sizeof(T);
  WriteBufferInternal(name, offset, outLength, writer);
}

void SaveBlendShapeHeader(const BlendShapeHeader& blendShapeHeader, JsonWriter& writer, ostream &outBin, unsigned int offset, unsigned int& outLenght)
{
  outLenght = sizeof(blendShapeHeader.width) + sizeof(blendShapeHeader.height);

  writer.WriteObject("blendShapeHeader", true);
  writer.WriteValue("version", BLEND_SHAPE_VERSION);
  writer.WriteValue("byteOffset", offset);
  writer.WriteValue("byteLength", outLenght);
  writer.CloseScope();

  outBin.write(reinterpret_cast<const char*>(&blendShapeHeader.width), sizeof(blendShapeHeader.width));
  outBin.write(reinterpret_cast<const char*>(&blendShapeHeader.height), sizeof(blendShapeHeader.height));
}

void SaveMeshes(Scene3D *scene, JsonWriter& outDli, ostream &outBin,
    std::string fileNameBin)
{
  unsigned int offset = 0;
  unsigned int length = 0;
  for (unsigned int m = 0; m < scene->GetNumMeshes(); m++)
  {
    const Mesh* mesh = scene->GetMesh(m);

    outDli.WriteObject(nullptr);
    unsigned int attributes = 0;
    attributes |= (mesh->m_Indices.size() > 0) ? 1 : 0;
    attributes |= (mesh->m_Positions.size() > 0) ? 2 : 0;
    attributes |= (mesh->m_Normals.size() > 0) ? 4 : 0;
    attributes |= (mesh->m_Textures.size() > 0) ? 8 : 0;
    attributes |= (mesh->m_Tangents.size() > 0) ? 16 : 0;
    // NOTE: bit 5 (note the zero-based index) used to be for bitangents.
    attributes |= (mesh->m_Joints0.size() > 0) ? 64 : 0;
    attributes |= (mesh->m_Weights0.size() > 0) ? 128 : 0;

    outDli.WriteValue("uri", fileNameBin.c_str());
    outDli.WriteValue("attributes", attributes);
    outDli.WriteValue("primitive", "TRIANGLES");

    offset += length;
    WriteBuffer<unsigned short>("indices", offset, mesh->m_Indices.size(), outDli, length);

    outBin.write((char*) mesh->m_Indices.data(), length);

    offset += length;
    WriteBuffer<Vector3>("positions", offset, mesh->m_Positions.size(), outDli, length);

    outBin.write((char*) mesh->m_Positions.data(), length);

    if (mesh->m_Normals.size())
    {
      offset += length;
      WriteBuffer<Vector3>("normals", offset, mesh->m_Normals.size(), outDli, length);

      outBin.write((char*) mesh->m_Normals.data(), length);
    }

    if (mesh->m_Textures.size())
    {
      offset += length;
      WriteBuffer<Vector2>("textures", offset, mesh->m_Textures.size(), outDli, length);

      outBin.write((char*) mesh->m_Textures.data(), length);
    }

    if (mesh->m_Tangents.size())
    {
      offset += length;
      WriteBuffer<Vector3>("tangents", offset, mesh->m_Tangents.size(), outDli, length);

      outBin.write((char*) mesh->m_Tangents.data(), length);
    }

    // write weights
    if (mesh->IsSkinned())
    {
      offset += length;
      WriteBuffer<Vector4>("joints0", offset, mesh->m_Joints0.size(), outDli, length);

      outBin.write(reinterpret_cast<const char*>(mesh->m_Joints0.data()), length);

      offset += length;
      WriteBuffer<Vector4>("weights0", offset, mesh->m_Weights0.size(), outDli, length);

      outBin.write(reinterpret_cast<const char*>(mesh->m_Weights0.data()), length);

      outDli.WriteValue("skeleton", scene->FindSkeletonId(mesh->m_Skeleton));
    }

  if(!mesh->m_BlendShapes.empty())
  {
    offset += length;
    SaveBlendShapeHeader(mesh->m_BlendShapeHeader, outDli, outBin, offset, length);

    std::vector<std::vector<Vector3>> positions;
    positions.resize(mesh->m_BlendShapes.size());

    outDli.WriteArray("blendShapes");

    const unsigned int numberOfVertices = mesh->m_Positions.size();
    float maxDistance = 0.f;

    uint32_t blendShapeIndex = 0u;
    for (auto blendShape : mesh->m_BlendShapes)
    {
      if (!blendShape.m_Positions.empty() && (numberOfVertices == blendShape.m_Positions.size()))
      {
        // Calculate the difference with the original mesh.
        // Find the max distance to normalize the deltas.
        std::vector<Vector3>& positionDeltas = positions[blendShapeIndex++];
        positionDeltas.resize(numberOfVertices);
        for (unsigned int index = 0u; index < numberOfVertices; ++index)
        {
          Vector3& delta = positionDeltas[index];
          const Vector3& originalValue = mesh->m_Positions[index];
          const Vector3& blendShapeValue = blendShape.m_Positions[index];
          delta = blendShapeValue - originalValue;

          maxDistance = std::max(maxDistance, delta.squareMagnitude());
        }
      }
    }

    const float normalizeFactor = (fabsf(maxDistance) < Util::EPSILON) ? 1.f : (0.5f / sqrtf(maxDistance));
    const float unnormalizeFactor = 1.f / normalizeFactor;

    blendShapeIndex = 0u;
    for (auto blendShape : mesh->m_BlendShapes)
    {
      outDli.WriteObject(nullptr);
      outDli.WriteValue("name", blendShape.m_Name.c_str());
      outDli.WriteValue("weight", blendShape.m_Weight);

      std::vector<Vector3>& positionDeltas = positions[blendShapeIndex++];
      if (!positionDeltas.empty() && (numberOfVertices == positionDeltas.size()))
      {
        // Normalize all the deltas and translate to a possitive value.
        // Deltas are going to be passed to the shader in a color texture
        // whose values that are less than zero are clamped.

        const unsigned int bufferSize = sizeof(Vector3) * numberOfVertices;
        std::vector<unsigned char> positionDeltasBuffer(bufferSize);
        for (unsigned int index = 0u, bufferIndex = 0u; index < numberOfVertices; ++index)
        {
          Vector3 delta = positionDeltas[index];
          delta.x = Util::clamp(((delta.x * normalizeFactor) + 0.5f), 0.f, 1.f);
          delta.y = Util::clamp(((delta.y * normalizeFactor) + 0.5f), 0.f, 1.f);
          delta.z = Util::clamp(((delta.z * normalizeFactor) + 0.5f), 0.f, 1.f);

          memcpy(positionDeltasBuffer.data()+bufferIndex, delta.data, sizeof(Vector3));
          bufferIndex += sizeof(Vector3);
        }

        offset += length;
        WriteBuffer<char>("positions", offset, bufferSize, outDli, length);
        outBin.write(reinterpret_cast<char*>(positionDeltasBuffer.data()), length);
      }

      if (!blendShape.m_Normals.empty() && (numberOfVertices == blendShape.m_Normals.size()))
      {
        // Calculate the difference with the original mesh, and translate to make all values positive.
        const unsigned int bufferSize = sizeof(Vector3) * numberOfVertices;
        std::vector<unsigned char> normalDeltasBuffer(bufferSize);
        for (unsigned int index = 0u, bufferIndex = 0u; index < numberOfVertices; ++index)
        {
          const Vector3& originalValue = mesh->m_Normals[index];
          const Vector3& blendShapeValue = blendShape.m_Normals[index];
          Vector3 delta = blendShapeValue - originalValue;

          delta.x *= 0.5f;
          delta.y *= 0.5f;
          delta.z *= 0.5f;

          delta.x += 0.5f;
          delta.y += 0.5f;
          delta.z += 0.5f;

          memcpy(normalDeltasBuffer.data()+bufferIndex, delta.data, sizeof(Vector3));
          bufferIndex += sizeof(Vector3);
        }

        offset += length;
        WriteBuffer<char>("normals", offset, bufferSize, outDli, length);
        outBin.write(reinterpret_cast<char*>(normalDeltasBuffer.data()), length);
      }

      if (!blendShape.m_Tangents.empty() && (numberOfVertices == blendShape.m_Tangents.size()))
      {
        // Calculate the difference with the original mesh, and translate to make all values positive.
        const unsigned int bufferSize = sizeof(Vector3) * numberOfVertices;
        std::vector<unsigned char> tangentDeltasBuffer(bufferSize);
        for (unsigned int index = 0u, bufferIndex = 0u; index < numberOfVertices; ++index)
        {
          const Vector3& originalValue = mesh->m_Normals[index];
          const Vector3& blendShapeValue = blendShape.m_Normals[index];
          Vector3 delta = blendShapeValue - originalValue;

          delta.x *= 0.5f;
          delta.y *= 0.5f;
          delta.z *= 0.5f;

          delta.x += 0.5f;
          delta.y += 0.5f;
          delta.z += 0.5f;

          memcpy(tangentDeltasBuffer.data()+bufferIndex, delta.data, sizeof(Vector3));
          bufferIndex += sizeof(Vector3);
        }

        offset += length;
        WriteBuffer<char>("tangents", offset, bufferSize, outDli, length);
        outBin.write(reinterpret_cast<char*>(tangentDeltasBuffer.data()), length);
      }
      outDli.CloseScope();
    }

    // Write the unnormalize factor.
    outBin.write(reinterpret_cast<const char*>(&unnormalizeFactor), sizeof(float));
    length += sizeof(float);
    outDli.CloseScope();
  }
    outDli.CloseScope();
  }
}

void SaveSkeletons(Scene3D* scene, JsonWriter& outDli)
{
    outDli.WriteArray("skeletons");
    for (unsigned int i = 0; i < scene->GetNumSkeletonRoots(); ++i)
    {
        outDli.WriteObject(nullptr, true);

        auto nodeName = Node3D::MakeValidName(scene->GetSkeletonRoot(i)->m_Name);
        outDli.WriteValue("node", nodeName.c_str());
        outDli.CloseScope();
    }

    outDli.CloseScope();
}

void SaveCameras(Scene3D *scene, JsonWriter& outDli)
{
  if (!scene->GetNumCameras())
  {
    outDli.WriteObject(nullptr);
    outDli.WriteValue("fov", 60.0);
    outDli.WriteValue("near", 0.1);
    outDli.WriteValue("far", 100.0);

    outDli.WriteArray("matrix", true);
    const double matrix[] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 3.5, 1.0 };
    WriteArrayData(matrix, outDli);
    outDli.CloseScope();

    outDli.CloseScope();
  }

  for (unsigned int i = 0; i < scene->GetNumCameras(); i++)
  {
    Camera3D *cam = scene->GetCamera(i);
    outDli.WriteObject(nullptr);
    outDli.WriteValue("fov", cam->fov);
    outDli.WriteValue("near", cam->near);
    outDli.WriteValue("far", cam->far);

    outDli.WriteArray("matrix", true);
    WriteArrayData(cam->m_Matrix, outDli);
    outDli.CloseScope();

    outDli.CloseScope();
  }
}

void SaveLights(Scene3D* scene, JsonWriter& outDli)
{
  for (unsigned int i = 0; i < scene->GetNumLights(); ++i)
  {
    const Light* light = scene->GetLight(i);

    outDli.WriteObject(nullptr);
    outDli.WriteArray("matrix", true);
    WriteArrayData(light->m_Matrix, outDli);
    outDli.CloseScope();

    outDli.WriteArray("color", true);
    WriteArrayData(light->m_DiffuseColor.data, outDli);
    outDli.CloseScope();

    outDli.CloseScope();
  }
}

void SaveMaterials(Scene3D *scene, JsonWriter& outDli)
{
  outDli.WriteObject(nullptr);
  outDli.WriteValue("texture1", "Basic_albedo_metallic.png");
  outDli.WriteValue("texture2", "Basic_normal_roughness.png");
  outDli.WriteValue("mipmap", true);
  outDli.WriteValue("environment", 1);
  outDli.CloseScope();
}

void SaveEnvironment(Scene3D *scene, JsonWriter& outDli)
{
  outDli.WriteObject(nullptr);
  outDli.CloseScope();
  outDli.WriteObject(nullptr);
  outDli.WriteValue("cubeSpecular", "Studio/Studio_Radiance.ktx");
  outDli.WriteValue("cubeDiffuse", "Studio/Studio_Irradiance.ktx");
  outDli.CloseScope();
}

void SaveShaders(Scene3D *scene, JsonWriter& outDli)
{
  outDli.WriteObject(nullptr);
  outDli.WriteValue("vertex", "default_pbr_shader.vsh");
  outDli.WriteValue("fragment", "default_pbr_shader.fsh");
  outDli.WriteValue("rendererState", "DEPTH_TEST|DEPTH_WRITE|CULL_BACK|DEPTH_FUNC:LESS_EQUAL");
  outDli.WriteArray("uCubeMatrix", true);
  const double matrix[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
  WriteArrayData(matrix, outDli);
  outDli.CloseScope();
  outDli.WriteValue("uMaxLOD", 6);
  outDli.CloseScope();
}

/**
 * @brief Saves animation as text, the default is saved as binary animation, this function is kept in case
 * to see the keyframes values for debugging purposes.
 */
void SaveAnimations(Scene3D *scene, JsonWriter& outDli, std::set<std::string>& animNames)
{
  for (unsigned int a = 0; a < scene->GetNumAnimations(); a++)
  {
    Animation3D *animation = scene->GetAnimation(a);
    outDli.WriteObject(nullptr);

    outDli.WriteValue("name", animation->Name.c_str());
    if (!animNames.insert(animation->Name).second)
    {
      std::cout << "Animation tag '" << animation->Name << "' is not unique" << std::endl;
    }

    outDli.WriteArray("properties");

    unsigned int weightIndex = 0u;
    std::string currentNodeName;
    for (unsigned int n = 0; n < animation->AnimNodesList.size(); n++)
    {
      NodeAnimation3D nodeAnim = animation->AnimNodesList[n];

      WriteNodeKeyframes("orientation", outDli, animation, n, nodeAnim.Rotations, 4 * sizeof(float));
      WriteNodeKeyframes("position", outDli, animation, n, nodeAnim.Positions, 3 * sizeof(float));
      WriteNodeKeyframes("scale", outDli, animation, n, nodeAnim.Scales, 3 * sizeof(float));
      if (!nodeAnim.Weights.empty())
      {
        if (currentNodeName != nodeAnim.NodeName)
        {
          currentNodeName = nodeAnim.NodeName;
          weightIndex = 0u;
        }
        char propertyName[256];
        sprintf(propertyName, "uBlendShapeWeight[%d]", weightIndex);;
        WriteNodeKeyframes(propertyName, outDli, animation, n, nodeAnim.Weights, sizeof(float));
        ++weightIndex;
      }
    }

    outDli.CloseScope();

    outDli.WriteValue("loopCount", 0);
    outDli.CloseScope();
  }
}

///@brief Abstracts the recording of data into a stream for multiple files,
/// processed sequentially.
class IRecorder
{
public:
  virtual ~IRecorder()
  {}

  ///@brief Starts recording a file with the given @a name.
  virtual void Start(const std::string& name) = 0;

  ///@return An ostream into which data should be written to.
  ///@note Normally, no reference to this stream should be kept around
  /// past a Finish() call.
  virtual std::ostream& GetStream() = 0;

  ///@brief Concludes recording the file.
  virtual void Finish() = 0;
};

void SaveAnimationsBinary(Scene3D *scene, JsonWriter& outDli, std::string outPath, std::set<std::string>& animNames, AnimationDataMap* animationContents)
{
  std::unique_ptr<IRecorder> recorder;
  if (animationContents)
  {
    class MapRecorder : public IRecorder
    {
    public:
      MapRecorder(AnimationDataMap& anims)
      : m_Anims(anims)
      {}

      virtual void Start(const std::string& name) override
      {
        m_CurrentStream.str("");
        m_CurrentName = name;
      }

      virtual std::ostream& GetStream() override
      {
        return m_CurrentStream;
      }

      virtual void Finish() override
      {
        m_Anims[m_CurrentName] = m_CurrentStream.str();
      }

    private:
      std::ostringstream m_CurrentStream;
      std::string m_CurrentName;
      AnimationDataMap& m_Anims;
    };

    recorder.reset(new MapRecorder(*animationContents));
  }
  else
  {
    class FileRecorder : public IRecorder
    {
    public:
      FileRecorder(const std::string& absolutePath)
      : m_AbsolutePath(absolutePath)
      {}

      virtual void Start(const std::string& name) override
      {
        assert(!m_Stream.is_open());
        m_Stream.open(m_AbsolutePath + name, std::ios::binary);
      }

      virtual std::ostream& GetStream() override
      {
        return m_Stream;
      }

      virtual void Finish() override
      {
        m_Stream.close();
      }

    private:
      std::string m_AbsolutePath;
      std::ofstream m_Stream;
    };

    recorder.reset(new FileRecorder(outPath));
  }

  for (unsigned int a = 0; a < scene->GetNumAnimations(); a++)
  {
    Animation3D *animation = scene->GetAnimation(a);
    if (animation->AnimNodesList.size() == 0)
    {
      continue;
    }

    outDli.WriteObject(nullptr);
    unsigned int offset = 0;

    outDli.WriteValue("name", animation->Name.c_str());
    if (!animNames.insert(animation->Name).second)
    {
      std::cout << "Animation tag '" << animation->Name << "' is not unique" << std::endl;
    }

    std::ostringstream stringStream;
    //append iteration number "a" to avoid cases with empty names or
    //repeated names
    stringStream << animation->Name << a << ".ani";
    std::string animationFilename = stringStream.str();

    outDli.WriteArray("properties");

    recorder->Start(animationFilename);

    unsigned int weightIndex = 0u;
    std::string currentNodeName;
    auto& osBin = recorder->GetStream();
    for (unsigned int n = 0; n < animation->AnimNodesList.size(); n++)
    {
      const NodeAnimation3D& nodeAnim = animation->AnimNodesList[n];
      WriteNodeKeyframesBin(animationFilename, "orientation", outDli, osBin, offset, animation, n, nodeAnim.Rotations, 4 * sizeof(float));
      WriteNodeKeyframesBin(animationFilename, "position", outDli, osBin, offset, animation, n, nodeAnim.Positions, 3 * sizeof(float));
      WriteNodeKeyframesBin(animationFilename, "scale", outDli, osBin, offset, animation, n, nodeAnim.Scales, 3 * sizeof(float));

      if (!nodeAnim.Weights.empty())
      {
        if (currentNodeName != nodeAnim.NodeName)
        {
          currentNodeName = nodeAnim.NodeName;
          weightIndex = 0u;
        }

        char propertyName[256];
        sprintf(propertyName, "uBlendShapeWeight[%d]", weightIndex);;
        WriteNodeKeyframesBin(animationFilename, propertyName, outDli, osBin, offset, animation, n, nodeAnim.Weights, sizeof(float));
        ++weightIndex;
      }
    }
    recorder->Finish();

    outDli.CloseScope();
    outDli.WriteValue("loopCount", 0);
    outDli.CloseScope();
  }
}

void WriteNodeKeyframes(const std::string& strProperty, JsonWriter& outDli, Animation3D *animation, int animationidx, const std::vector<NodeKey>& keyframes, unsigned int keyByteSize)
{
  if (keyframes.size())
  {
    const NodeAnimation3D& nodeAnim = animation->AnimNodesList[animationidx];
    outDli.WriteObject(nullptr);

    auto nodeName = Node3D::MakeValidName(nodeAnim.NodeName);
    outDli.WriteValue("node", nodeName.c_str());
    outDli.WriteValue("property", strProperty.c_str());

    outDli.WriteObject("timePeriod", true);
    outDli.WriteValue("delay", 0.0);
    outDli.WriteValue("duration", animation->Duration / animation->TicksPerSecond);
    outDli.CloseScope();

    outDli.WriteArray("keyFrames");

    for (unsigned int k = 0; k < keyframes.size(); k++)
    {
      const NodeKey& nkey = keyframes[k];
      outDli.WriteObject(nullptr, true);
      outDli.WriteValue("progress", nkey.time / animation->Duration);
      outDli.WriteArray("value", true);
      WriteArrayData(nkey.v, keyByteSize / sizeof(float), outDli);
      outDli.CloseScope();
      outDli.CloseScope();
    }

    outDli.CloseScope();
    outDli.CloseScope();
  }
}

void WriteNodeKeyframesBin(const std::string& url, const std::string& strProperty, JsonWriter& outDli, std::ostream& osBin, unsigned int& offset, Animation3D *animation, int animationidx, const std::vector<NodeKey>& keyframes, unsigned int keyByteSize)
{
  if (keyframes.size())
  {
    const NodeAnimation3D& nodeAnim = animation->AnimNodesList[animationidx];
    outDli.WriteObject(nullptr);

    auto nodeName = Node3D::MakeValidName(nodeAnim.NodeName);
    outDli.WriteValue("node", nodeName.c_str());
    outDli.WriteValue("property", strProperty.c_str());

    outDli.WriteObject("timePeriod", true);
    outDli.WriteValue("delay", 0.0);
    outDli.WriteValue("duration", animation->Duration / animation->TicksPerSecond);
    outDli.CloseScope();

    outDli.WriteObject("keyFramesBin", true);
    outDli.WriteValue("url", url.c_str());
    outDli.WriteValue("byteOffset", offset);

    for (unsigned int k = 0; k < keyframes.size(); k++)
    {
      const NodeKey& nkey = keyframes[k];
      float progress = nkey.time / animation->Duration;
      osBin.write((char*)(&progress), sizeof(float));
      osBin.write((char*)(nkey.v), keyByteSize);
      osBin.write(&nkey.alphaFunction, sizeof(unsigned char));
    }
    offset += (keyByteSize + sizeof(float) + sizeof(unsigned char)) *  keyframes.size();
    outDli.WriteValue("numKeys", static_cast<uint32_t>(keyframes.size()));
    outDli.CloseScope();

    outDli.CloseScope();
  }
}
