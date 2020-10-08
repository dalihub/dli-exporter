#ifndef LIBDLI_ANIMATED_IMAGE_STATE_H_
#define LIBDLI_ANIMATED_IMAGE_STATE_H_

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

// INTERNAL INCLUDESs
#include "index.h"

// EXTERNAL INCLUDES
#include "dali/public-api/actors/actor.h"
#include "dali/public-api/adaptor-framework/timer.h"
#include <functional>
#include <memory>

namespace dli
{

class LIBDLI_API AnimatedImage : public Dali::ConnectionTracker
{
public:
  using Ptr = std::unique_ptr<AnimatedImage>;
  using UpdateAnimatedMaterialFn = std::function<bool(Dali::Actor, Index, int)>;

  AnimatedImage(Dali::Actor actor, Index materialIndex, int frameRate, int frameStart, int frameEnd, int loopCount, UpdateAnimatedMaterialFn callback);
  const Dali::Actor GetActor() const;

  void Play();
  void Pause();
  void Resume();
  void Stop();

private:
  bool OnTimer();
  bool IncreaseFrame();
  bool IncreaseLoopCount();
  void Reset();

  Dali::Timer mTimer;

  Dali::Actor mActor;
  Index mMaterialIndex;
  int mFrameRate;
  int mFrameStart;
  int mFrameEnd;
  int mFrameCurrent;
  int mLoopCount;
  int mLoopCountCurrent;
  UpdateAnimatedMaterialFn mCallback;
};

}

#endif //LIBDLI_ANIMATED_IMAGE_STATE_H_

