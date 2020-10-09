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
#include "libdli/animation-definition.h"

using namespace Dali;

namespace dli
{

const float AnimationDefinition::DEFAULT_DURATION_SECONDS = 1.f;
const float AnimationDefinition::MIN_DURATION_SECONDS = 1e-2f;

Animation::EndAction AnimationDefinition::StopForModification(Animation& anim)
{
  const auto endAction = anim.GetEndAction();
  anim.SetEndAction(Animation::DISCARD);
  anim.Stop();
  return endAction;
}

AnimationDefinition::AnimationDefinition()
{}

AnimationDefinition::AnimationDefinition(AnimationDefinition&& other)
:  mName(std::move(other.mName)),
  mDuration(other.mDuration),
  mLoopCount(other.mLoopCount),
  mDisconnectAction(other.mDisconnectAction),
  mEndAction(other.mEndAction),
  mSpeedFactor(other.mSpeedFactor),
  mPlayRange(other.mPlayRange),
  mProperties(std::move(other.mProperties))
{}

void AnimationDefinition::Animate(Animation& animation, AnimatedProperty::GetActor getActor)
{
  DALI_ASSERT_ALWAYS(animation);
  for (auto& ap : mProperties)
  {
    ap.Animate(animation, getActor);
  }
}

Animation AnimationDefinition::ReAnimate(AnimatedProperty::GetActor getActor)
{
  // Create and configure new animation.
  Animation a = Animation::New(mDuration);
  a.SetLoopCount(mLoopCount);
  a.SetDisconnectAction(mDisconnectAction);
  a.SetEndAction(mEndAction);

  a.SetSpeedFactor(mSpeedFactor);
  a.SetPlayRange(mPlayRange);

  Animate(a, getActor);
  return a;
}

AnimationDefinition& AnimationDefinition::operator=(AnimationDefinition&& other)
{
  AnimationDefinition tmp(std::move(other));
  mName = std::move(tmp.mName);
  mDuration = tmp.mDuration;
  mLoopCount = tmp.mLoopCount;
  mDisconnectAction = tmp.mDisconnectAction;
  mEndAction = tmp.mEndAction;
  mSpeedFactor = tmp.mSpeedFactor;
  mPlayRange = tmp.mPlayRange;
  mProperties.swap(tmp.mProperties);
  return *this;
}

}
