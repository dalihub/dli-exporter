#ifndef VECTOR4_H
#define VECTOR4_H
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

template <typename T>
struct TVector4
{
    union
    {
        struct {
            T x;
            T y;
            T z;
            T w;
        };
        T data[4];
    };
};

using Vector4 = TVector4<float>;
using IVector4 = TVector4<int>;

#endif // VECTOR4_H
