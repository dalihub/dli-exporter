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
#include "Matrix.h"
#include "Util.h"

#include <numeric>

const float Matrix::IDENTITY[] = { 1.0f, .0f, .0f, .0f, .0f, 1.0f, .0f, .0f, .0f, .0f, 1.0f, .0f, .0f, .0f, .0f, 1.0f };


void Matrix::SetMatrix(const aiMatrix4x4 & in, float out[NUM_ELEMENTS])
{
  out[0] = Util::trim(in.a1);
  out[1] = Util::trim(in.b1);
  out[2] = Util::trim(in.c1);
  out[3] = Util::trim(in.d1);

  out[4] = Util::trim(in.a2);
  out[5] = Util::trim(in.b2);
  out[6] = Util::trim(in.c2);
  out[7] = Util::trim(in.d2);

  out[8] = Util::trim(in.a3);
  out[9] = Util::trim(in.b3);
  out[10] = Util::trim(in.c3);
  out[11] = Util::trim(in.d3);

  out[12] = Util::trim(in.a4);
  out[13] = Util::trim(in.b4);
  out[14] = Util::trim(in.c4);
  out[15] = Util::trim(in.d4);
}

Matrix::Matrix(const float source[NUM_ELEMENTS])
{
    if (source)
    {
        std::copy(source, source + NUM_ELEMENTS, data);
    }
}

Matrix::Matrix(const aiMatrix4x4 & matrix)
{
    SetMatrix(matrix, data);
}

bool Matrix::IsIdentity() const
{
    auto p = IDENTITY;
    float delta = std::accumulate(data, data + NUM_ELEMENTS, .0f,
        [&p](float v0, float v1) {
        float delta = std::abs(*(p++) - v1);
        return v0 + delta;
    });
    return delta < 1e-6;
}
