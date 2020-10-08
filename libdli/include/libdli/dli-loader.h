#ifndef LIBDLI_DLI_LOADER_H
#define LIBDLI_DLI_LOADER_H
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

 // INTERNAL INCLUDES
#include "libdli-api.h"
#include "animation-definition.h"
#include "lighting-mode.h"
#include "text-cache-item.h"
#include "customization.h"
#include "string-callback.h"
#include "index.h"

// EXTERNAL INCLUDES
#include "dali/public-api/common/vector-wrapper.h"
#include "dali/devel-api/common/map-wrapper.h"

#include "dali-toolkit/devel-api/builder/json-parser.h"

namespace dli
{
typedef std::pair<std::string, std::string> Metadata;

// Forward declarations
struct LoadResult;
struct CameraParameters;
struct LightParameters;
struct TextParameters;

class ResourceBundle;
struct NodeDefinition;
class SceneDefinition;

class LIBDLI_API DliLoader
{
public:
  using ConvertFontCode = void(*)(const std::string& code, std::string& fontFamily, std::string& slant, std::string& weight, float& size);
  using ConvertColorCode = Dali::Vector4(*)(const std::string& code);

  using CategoryProcessor = std::function<void(const Dali::Toolkit::TreeNode* tnCategory, StringCallback onError)>;
  using CategoryProcessorVector = std::vector<std::pair<std::string /*name*/, CategoryProcessor>>;

  using NodeProcessor = std::function<void(const NodeDefinition& nodeDef,
    const Dali::Toolkit::TreeNode& tnNode, StringCallback onError)>;

  using AnimationProcessor = std::function<void(const AnimationDefinition& animDef,
    const Dali::Toolkit::TreeNode& tnAnim, StringCallback onError)>;

  struct InputParams
  {
    ///@brief The absolute path of animation binaries referenced in the .dli.
    std::string mAnimationsPath;

    ///@brief Provides a facility to determine font family, slant, weight and size
    /// from the "code" property of a "text" node.
    ConvertFontCode mConvertFontCode;

    ///@brief Provides a facility to determine font color from the "code" property
    /// of a "text" node.
    ConvertColorCode mConvertColorCode;

    ///@brief Provides a text localisation facility.
    TextCacheItem::Localizer mGetLocalisedText;

    ///@brief A collection of handlers, mapped to the names of the top level (i.e. below
    /// root) element, whom they will attempt to process. This will take place before
    /// the parsing of scene Nodes and Animations, but after skeletons, environment, mesh,
    /// shader and material resources.
    CategoryProcessorVector mPreNodeCategoryProcessors;

    ///@brief A collection of handlers, mapped to the names of the top level (i.e. below
    /// root) element, whom they will attempt to process. This will take place after
    /// the parsing of the scene Nodes and Animations.
    CategoryProcessorVector mPostNodeCategoryProcessors;

    ///@brief Provides an extension point to nodes. If provided, this function will be
    /// called with each JSON element and definition, of a scene node.
    ///@note Constraints rely on ID resolution (from .dli to scene definition), which
    /// takes place after the parsing of the nodes; therefore AT THIS POINT the node
    /// IDs seen in constraints will still be the .dli IDs - NOT to be relied on for
    /// indexing into mScene.
    NodeProcessor mNodePropertyProcessor;

    ///@brief Provides an extension point to animations. If provided, this function will be
    /// called with each JSON element and fully processed definition, of an animation.
    AnimationProcessor mAnimationPropertyProcessor;
  };

  struct LoadParams
  {
    InputParams const& input;
    LoadResult& output;
  };

  ///@brief Sets the callback that messages from non-fatal errors get posted to.
  /// Uses DefaultErrorCallback by default.
  void SetErrorCallback(StringCallback onError);

  ///@brief Attempts to load and parse a .dli document into a DOM tree.
  ///@return Whether the operation was successful.
  bool LoadScene(const std::string& uri, LoadParams& params);

  std::string GetParseError() const;

  void ParseShaders(const Dali::Toolkit::TreeNode* shaders, ResourceBundle& resources);
  void ParseMeshes(const Dali::Toolkit::TreeNode* meshes, ResourceBundle& resources);

private:
  ///@brief Due to .dli nodes being processed in depth-first traversal with orphans being
  /// ignored, features that rely on node indices (which is more compact and closer to
  /// glTF) require a mapping from .dli node indices to those in the resulting SceneDefinition.
  /// The index mapper is responsible for maintaing this mapping, and resolving node IDs
  /// once the processing of the nodes has finished.
  ///@note The resolution requires the whole scene graph to finish parsing, therefore any
  /// node extensions relying on node IDs will see the dli ID in their processor.
  struct IIndexMapper
  {
    ///@brief Attempts to create a mapping from a node's @a dli index to its @a scene
    /// index.
    ///@return Whether the operation was successful.
    virtual bool Map(Index iDli, Index iScene) =0;

    ///@return The scene index for the node's @a dli index.
    virtual Index Resolve(Index iDli) =0;
  };

  ///@brief Traverses the DOM tree created by LoadDocument() in an attempt to create
  /// an intermediate representation of resources and nodes.
  void ParseScene(LoadParams& params);
  void ParseSceneInternal(Index iScene, const Dali::Toolkit::TreeNode* tnScenes,
    const Dali::Toolkit::TreeNode* tnNodes, LoadParams& params);

  void ParseSkeletons(const Dali::Toolkit::TreeNode* skeletons, SceneDefinition& scene, ResourceBundle& resources);
  void ParseEnvironments(const Dali::Toolkit::TreeNode* environments, ResourceBundle& resources);
  void ParseMaterials(const Dali::Toolkit::TreeNode* materials, ConvertColorCode convertColorCode,
    ResourceBundle& resources);

  void ParseNodes(const Dali::Toolkit::TreeNode* nodes, Index index, LoadParams& params);
  void ParseNodesInternal(const Dali::Toolkit::TreeNode* nodes, Index index,
    LightingMode::Type lightingMode, std::vector<Index>& inOutParentStack, LoadParams& params,
    IIndexMapper& indexMapper);

  void ParseAnimations(const Dali::Toolkit::TreeNode* animations, LoadParams& params);
  void ParseAnimationGroups(const Dali::Toolkit::TreeNode* animationGroups, LoadParams& params);

  void GetCameraParameters(std::vector<CameraParameters>& cameras) const;
  void GetLightParameters(std::vector<LightParameters>& lights) const;

private:
  StringCallback mOnError = DefaultErrorCallback;
  Dali::Toolkit::JsonParser mParser;

  std::map<Index, Dali::Matrix>  mInverseBindMatrices;
};

} // namespace dli

#endif // LIBDLI_DLI_LOADER_H
