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
#include <cmath>
#include "Util.h"

using namespace std;

const float Util::EPSILON = 1e-4f;

Util::Util()
{
}


Util::~Util()
{
}


float Util::trim(float value)
{
  return abs(value - round(value)) < EPSILON ? round(value) : value;
}

float Util::clamp(float v, float lo, float hi)
{
  v = (v < lo) ? lo : v;
  v = (v > hi) ? hi : v;

  return v;
}
