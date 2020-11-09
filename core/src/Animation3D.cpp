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
#include "Animation3D.h"

NodeAnimation3D::NodeAnimation3D()
{
    //ctor
}

NodeAnimation3D::~NodeAnimation3D()
{
    //dtor
}


Animation3D::Animation3D()
:TicksPerSecond(1.0f),
 Duration(0.0f),
 Name("loaded")
{
    //ctor
}

Animation3D::~Animation3D()
{
    //dtor
}

bool Animation3D::HasAnimations() const
{
    for( const auto& animNode : AnimNodesList )
    {
        if( !animNode.Rotations.empty() || !animNode.Positions.empty() || !animNode.Scales.empty() || !animNode.Weights.empty() )
        {
            return true;
        }
    }
    return false;
}
