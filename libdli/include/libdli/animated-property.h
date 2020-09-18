#ifndef LIBDLI_ANIMATED_PROPERTY_H
#define LIBDLI_ANIMATED_PROPERTY_H
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

#include "dali/public-api/actors/actor.h"
#include "dali/public-api/animation/animation.h"
#include "dali/public-api/object/property.h"
#include <memory>
#include <functional>

namespace dli
{

///@brief Intermediate representation for a property that's given to
/// a Dali::Animation to animate. Since there is no getting an
/// animated property back from the Animation (i.e. past AnimateBetween/By/To()),
/// changing properties (e.g. from the SDK) requires the whole Animation
/// object to be recreated with all of its properties (incl. modifications).
struct AnimatedProperty
{
public: // METHODS
  ///@brief Function to obtain an Actor based on its name. Its processing will
  /// ignore empty handles returned by it.
  using GetActor = std::function<Dali::Actor(const std::string&)>;

  ///@return The Property object (of the given @a actor) whose value is being animated.
  Dali::Property GetProperty(Dali::Actor& actor)
  {
    auto idx = actor.GetPropertyIndex(mPropertyName);
    return Dali::Property(actor, idx);
  }

  ///@brief The type of the Property (of the given @a actor) that is being animated.
  Dali::Property::Type GetPropertyType(Dali::Actor& actor)
  {
    auto idx = actor.GetPropertyIndex(mPropertyName);
    return actor.GetPropertyType(idx);
  }

  ///@brief Registers the animation of this property against the given @a anim.
  /// @a getActor will be used to obtain the Actor named by this property.
  /// Failing to find the actor purely means that this property will not be
  /// animated.
  void Animate(Dali::Animation& anim, GetActor getActor);

public: // DATA
  struct Value
  {
    Dali::Property::Value mValue;
    bool mIsRelative;
  };

  std::string mNodeName;
  std::string mPropertyName;

  Dali::KeyFrames mKeyFrames;
  std::unique_ptr<Value> mValue;

  Dali::AlphaFunction mAlphaFunction = Dali::AlphaFunction::DEFAULT;
  Dali::TimePeriod mTimePeriod = Dali::TimePeriod(0.f);
};

}

#endif //LIBDLI_ANIMATED_PROPERTY_H
