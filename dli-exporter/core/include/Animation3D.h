#ifndef ANIMATION3D_H
#define ANIMATION3D_H
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
#include <string>
#include <vector>

struct NodeKey
{
    /**
     * Note: this is section is reserved to set set the "alpha" function
     * Dali doesn't have "Step" alpha function.
     */
    enum alphaFunctionMode
    {
        STEP = 0,
        LINEAR = 1
    };

    NodeKey()
    {
        time = v[0] = v[1] = v[2] = v[3] = 0.0f;
        alphaFunction = LINEAR;
    }

    /**
     * time of keyframe
     */
    float time;

    /**
     * v[3] is w in quaternion and 0.0 for vector3
     */
    float v[4];

    /**
     * animation interpolation mode
     */
    char alphaFunction;
};

class NodeAnimation3D
{
    public:
        NodeAnimation3D();
        virtual ~NodeAnimation3D();

        std::string NodeName;
        std::vector<NodeKey> Rotations;
        std::vector<NodeKey> Positions;
        std::vector<NodeKey> Scales;
        std::vector<NodeKey> Weights;
    protected:

    private:

};

class Animation3D
{
    public:
        Animation3D();
        virtual ~Animation3D();
        bool HasAnimations() const;
        float TicksPerSecond;
        float Duration; // in Ticks
        std::string Name;
        std::vector<NodeAnimation3D> AnimNodesList;
    protected:

    private:

};

#endif // ANIMATION3D_H
