#ifndef VECTOR3_H
#define VECTOR3_H
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
#include <math.h>

struct Vector3
{
        Vector3()
        :x(0.0f),
         y(0.0f),
         z(0.0f)
         {

         }
        void normalize()
        {
            float temp = sqrtf(squareMagnitude());
            x /= temp;
            y /= temp;
            z /= temp;
        }

        float squareMagnitude() const
        {
            return x * x + y * y + z * z;
        }

        Vector3& operator - (const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;

            return *this;
        }

        Vector3 operator - (const Vector3& v) const
        {
            Vector3 result(*this);
            result.x -= v.x;
            result.y -= v.y;
            result.z -= v.z;

            return result;
        }

        union
        {
             struct {
                float x;
                float y;
                float z;
             };
             float data[3];
        };
};

#endif // VECTOR3_H
