#ifndef SAVESCENE_H
#define SAVESCENE_H
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

#include "Scene3D.h"
#include <map>

/**
 * @brief Saves the given @a scene to the given absolute paths for the .dli and
 *        .bin files.
 * @return The success of the operation.
 */
bool SaveScene(Scene3D* scene, std::string dliOutputPath,
    std::string binOutputPath, bool saveMaterials);

/**
 * @brief Performs conversion of the given @a scene, writing the .dli and .bin
 *        data to the provided streams.
 * @param fileNameBin is the intended filename for the binary file, which will
 *        be referenced from the .dli.
 * @param outDli Stream to write the gltf / dli data to.
 * @param outBin Stream to write the binary data to.
 * @param binaryAnimations to write binary animations, the default mode.
 * @param animationContents Optional; if it was provided, and binaryAnimations
 *        was set, the pair of names & content byte buffers of animation files
 *        will be registered into it instead of saved to the filesystem (into
 *        the folder which is the parent of @a fileNameBin).
 * @return The success of the operation.
 */
bool ConvertScene(Scene3D* scene, std::string fileNameBin, std::ostream& outDli,
    std::ostream& outBin, bool saveMaterials, bool binaryAnimations = true,
    std::map<std::string, std::string>* animationContents = nullptr);

#endif // SAVESCENE_H
