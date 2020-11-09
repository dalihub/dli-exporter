#ifndef CAMERA3D_H
#define CAMERA3D_H
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

#include "Vector3.h"
#include "assimp/scene.h"

class Camera3D
{
    public:
        Camera3D();
        virtual ~Camera3D();
        float fov;
        float near;
        float far;
        float m_Matrix[16];
        void MultLookAtMatrix( Vector3 look_atVector, Vector3 upVector, Vector3 positionVector );
        void SetMatrix(aiMatrix4x4 eMat);
    protected:

    private:
};

#endif // CAMERA3D_H
