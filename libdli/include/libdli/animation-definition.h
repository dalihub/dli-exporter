#ifndef LIBDLI_ANIMATION_DEFINITION_H
#define LIBDLI_ANIMATION_DEFINITION_H
/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS (Confidential Information).
 * You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability
 * of the software, either express or implied, including but not
 * limited to the implied warranties of merchantability, fitness for
 * a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as
 * a result of using, modifying or distributing this software or its derivatives.
 */

#include "libdli-api.h"
#include "animated-property.h"
#include "dali/public-api/common/vector-wrapper.h"

namespace dli
{

///@brief Animation handle + name + definition of properties.
class LIBDLI_API AnimationDefinition
{
public: // STATIC
  // For Animations created in the SDK.
  static const float DEFAULT_DURATION_SECONDS;

  // For parsing Animations from dli, when duration was not defined.
  static const float MIN_DURATION_SECONDS;

  ///@brief Saves the original end action of @a anim, sets the end action to
  /// Discard, then stops the animation and returns the end action.
  static Dali::Animation::EndAction StopForModification(Dali::Animation& anim);

public: // METHODS
  AnimationDefinition();

  AnimationDefinition(AnimationDefinition&& other);

  ///@brief Registers the properties against the given @a animation. @a getActor
  /// will be used to obtain the Actors for each AnimatedProperty.
  void Animate(Dali::Animation& animation, AnimatedProperty::GetActor getActor);

  ///@brief Creates a new Animation and Animates() its properties. @a getActor
  /// will be used to obtain the Actors for each AnimatedProperty.
  Dali::Animation ReAnimate(AnimatedProperty::GetActor getActor);

  AnimationDefinition& operator=(AnimationDefinition&& other);

public: // DATA
  std::string mName;

  float mDuration = DEFAULT_DURATION_SECONDS;
  int mLoopCount = 1;
  Dali::Animation::EndAction mDisconnectAction = Dali::Animation::BAKE_FINAL;
  Dali::Animation::EndAction mEndAction = Dali::Animation::BAKE;
  float mSpeedFactor = 1.f;
  Dali::Vector2 mPlayRange = Dali::Vector2{ 0.f, 1.f };

  std::vector<AnimatedProperty> mProperties;
};

struct AnimationGroupDefinition
{
  std::string mName;
  std::vector<std::string> mAnimations;
};

}

#endif //LIBDLI_ANIMATION_DEFINITION_H

