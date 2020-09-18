#ifndef LIBDLI_LIGHTING_MODE_H_
#define LIBDLI_LIGHTING_MODE_H_

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
#include <cstdint>
#include <string>

namespace dli
{

struct LightingMode
{
  enum Type
  {
    UNLIT,  // 0x00 - does not receive light, does not cast shadows;
    LIT,  // 0x01 - receives light and shadows;
    UNLIT_SOLID, // 0x02 = casts shadows, doesn't receive light;
    LIT_SOLID, // 0x03 - receives light and shadows and casts shadows.
  };
  
  static const char* const PROPERTY_NAME;

  static const char* GetValue(Type lm);
  static bool InterpretValue(const std::string& value, Type& lm);

  LightingMode() = delete;
};

}

#endif /* LIBDLI_LIGHTING_MODE_H_ */
