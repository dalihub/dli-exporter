#ifndef MATRIX_H
#define MATRIX_H
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
#include <algorithm>

#include "assimp/matrix4x4.h"

///@brief Column-major matrix.
struct Matrix
{
    enum { NUM_ELEMENTS = 16 };

    static const float IDENTITY[NUM_ELEMENTS];

    ///@brief Sets values from row-major AssImp matrix.
    static void SetMatrix(const aiMatrix4x4& in, float out[NUM_ELEMENTS]);

    Matrix(const float source[NUM_ELEMENTS] = IDENTITY);
    Matrix(const aiMatrix4x4& matrix);

    bool IsIdentity() const;

    float data[NUM_ELEMENTS];
};


#endif
