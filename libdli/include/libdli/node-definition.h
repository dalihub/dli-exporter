#ifndef LIBDLI_NODE_DEFINITION_H_
#define LIBDLI_NODE_DEFINITION_H_
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

// EXTERNAL
#include "dali-toolkit/devel-api/text/text-utils-devel.h"
#include "dali/public-api/math/quaternion.h"
#include "dali/public-api/math/matrix.h"
#include "dali/public-api/math/vector4.h"
#include "dali/public-api/actors/actor.h"
#include <string>
#include <memory>
#include <functional>

// INTERNAL
#include "lighting-mode.h"
#include "customization.h"
#include "matrix-stack.h"
#include "resource-bundle.h"
#include "text-cache-item.h"

namespace dli
{

class ViewProjection;

///@brief Interface to report (const) resource ids to.
class IResourceReceiver
{
public:
  virtual ~IResourceReceiver()
  {}

  virtual void Register(ResourceType::Value type, Index id) = 0;
};

///@brief Interface to report modifiable resource ids to.
///@note These are supposed to be transient. Obviously, the references collected
/// this way must not outlive the objects that they came from.
class IResourceReflector
{
public:
  virtual ~IResourceReflector()
  {}

  virtual void Reflect(ResourceType::Value type, Index& id) = 0;
};

///@brief Intermediate representation for a constraint that shall be
/// set up after the Actors were created. The target of the constraint
/// is the node definition that carries it.
struct ConstraintDefinition
{
  std::string mProperty;  ///< name of the property to constrain.
  Index mSourceIdx;  ///< index of the node to serve as the source of the constraint.

  bool operator<(const ConstraintDefinition& other) const
  {
    return mProperty < other.mProperty;
  }

  bool operator==(const ConstraintDefinition& other) const
  {
    return mSourceIdx == other.mSourceIdx && mProperty == other.mProperty;
  }

  bool operator!=(const ConstraintDefinition& other) const
  {
    return !operator==(other);
  }
};

struct Transforms
{
  MatrixStack modelStack;
  const ViewProjection& viewProjection;
};

///@brief Information about a skeleton and the shader that needs to be configured with it.
///@note Multiple skeletons shalt not share the same shader.
struct SkinningShaderConfigurationRequest
{
  Index mSkeletonIdx;
  Dali::Shader mShader;

  bool operator<(const SkinningShaderConfigurationRequest& other) const
  {
    return mShader < other.mShader;
  }
};

/**
 * @brief Needed to configure blend shape properties.
 */
struct BlendshapeShaderConfigurationRequest
{
  std::string mNodeName;
  Index mMeshIdx;
  Dali::Shader mShader;

  bool operator<(const BlendshapeShaderConfigurationRequest& other) const
  {
    return mShader < other.mShader;
  }
};

///@brief Request for creating a constraint, output from NodeDefinition::OnCreate.
struct ConstraintRequest
{
  const ConstraintDefinition* const mConstraint;  ///< Definition of the constraint to create.
  Dali::Actor mTarget;  ///< Target of the constraint.
};

///@brief Information about animated images that needs to be configured with it.
struct AnimatedImageNodeConfigurationRequest
{
  Dali::Actor mActor;
  Index mMaterialIndex;
  int mFrameStart;
  int mFrameEnd;
  int mFrameRate;
  int mLoopCount;
};

///@brief Defines a node, consisting of a name, a transform, a size, a lighting mode
/// (applied recursively), a list of child nodes, and slots for customization and
/// rendering logic, which are mutually exclusive in the current implementation.
struct NodeDefinition
{
public:  // TYPES
  using Vector = std::vector<NodeDefinition>;

  struct CreateParams
  {
  public: // input
    const ResourceBundle& mResources;
    Transforms& mXforms;

    TextCacheItem::Registrator mRegisterTextCacheItem;

  public: // output
    std::vector<ConstraintRequest> mConstrainables;
    std::vector<SkinningShaderConfigurationRequest> mSkinnables;
    std::vector<BlendshapeShaderConfigurationRequest> mBlendshapeRequests;
    std::vector<AnimatedImageNodeConfigurationRequest> mAnimatables;
  };

  class Renderable
  {
  public: // DATA
    Index mShaderIdx = INVALID_INDEX;

  public: // METHODS
    virtual ~Renderable() {}

    virtual void RegisterResources(IResourceReceiver& receiver) const;
    virtual void ReflectResources(IResourceReflector& reflector);
    virtual void OnCreate(const NodeDefinition& node, CreateParams& params, Dali::Actor& actor) const;
  };

  struct CustomizationDefinition
  {
    std::string mTag;

    Index GetChildId(const Customization::Choices& choices, const NodeDefinition& node)
    {
      auto iFind = choices.find(mTag);
      return std::min(iFind != choices.end() ? iFind->second : 0,
        static_cast<Index>(node.mChildren.size() - 1));
    }
  };

  class IVisitor
  {
  public:
    virtual void Start(NodeDefinition& n) = 0;
    virtual void Finish(NodeDefinition& n) = 0;

  protected:
    ~IVisitor() {}
  };

  class IConstVisitor
  {
  public:
    virtual void Start(const NodeDefinition& n) = 0;
    virtual void Finish(const NodeDefinition& n) = 0;

  protected:
    ~IConstVisitor() {}
  };

  struct Extra
  {
    std::string mKey;
    Dali::Property::Value mValue;

    bool operator<(const Extra& other) const
    {
      return mKey < other.mKey;
    }
  };

public:  // METHODS
  ///@brief Creates a DALi Actor from this definition only.
  ///@note Not recursive.
  Dali::Actor CreateActor(CreateParams& params) const;

  Dali::Matrix GetLocalSpace() const;

public: // DATA
  static const std::string ORIGINAL_MATRIX_PROPERTY_NAME;

  std::string mName;

  Dali::Vector3 mPosition = Dali::Vector3::ZERO;
  Dali::Quaternion mOrientation = Dali::Quaternion::IDENTITY;
  Dali::Vector3 mScale = Dali::Vector3::ONE;
  Dali::Vector3 mSize = Dali::Vector3::ONE;

  bool mIsVisible = true;

  LightingMode::Type mLightingMode = LightingMode::UNLIT;

  std::unique_ptr<Renderable> mRenderable;
  std::unique_ptr<CustomizationDefinition> mCustomization;
  std::vector<Extra> mExtras;
  std::vector<ConstraintDefinition> mConstraints;

  std::vector<Index> mChildren;
  Index mParentIdx = INVALID_INDEX;
};

class ModelNode : public NodeDefinition::Renderable
{
public: // DATA
  Dali::Vector4 mColor = Dali::Color::WHITE;
  Index mMeshIdx = INVALID_INDEX;
  Index mMaterialIdx = INVALID_INDEX;

public: // METHODS
  void RegisterResources(IResourceReceiver& receiver) const override;
  void ReflectResources(IResourceReflector& reflector) override;
  void OnCreate(const NodeDefinition& node, NodeDefinition::CreateParams& params, Dali::Actor& actor) const override;
};

///@brief Parameters for an Arc node.
class ArcNode : public ModelNode
{
public: // DATA
  bool mAntiAliasing = true;
  int mArcCaps = 0;
  float mStartAngleDegrees = .0f;
  float mEndAngleDegrees = .0f;
  float mRadius = .0f;

public: // METHODS
  static void GetEndVectorWithDiffAngle(float startAngle, float endAngle, Dali::Vector2& endVector);

  void OnCreate(const NodeDefinition& node, NodeDefinition::CreateParams& params, Dali::Actor& actor) const override;
};

///@brief Parameters for a Text Node.
class TextNode : public NodeDefinition::Renderable
{
public:
  void OnCreate(const NodeDefinition& node, NodeDefinition::CreateParams& params, Dali::Actor& actor) const override;

public: // DATA
  std::string mInternationalizationTextCode;

  Dali::Toolkit::DevelText::RendererParameters mStyle;
  Dali::Vector4 mShadowColor = Dali::Color::BLACK;
  Dali::Vector2 mShadowOffset = Dali::Vector2::ZERO;
  float mRadius = 0.f;
};

///@brief Parameters for an Animated Image Node.
class AnimatedImageNode : public ModelNode
{
public:  // METHODS
  void OnCreate(const NodeDefinition& node, NodeDefinition::CreateParams& params, Dali::Actor& actor) const override;

public:  // DATA
  int mFrameStart = 0;
  int mFrameEnd = -1;
  int mFrameRate = 60;
  int mLoopCount = 0;
};

}

#endif //LIBDLI_NODE_DEFINITION_H_