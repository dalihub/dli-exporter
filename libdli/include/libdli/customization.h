#ifndef LIBDLI_CUSTOMIZATION_STATE_H_
#define LIBDLI_CUSTOMIZATION_STATE_H_

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

 // INTERNAL INCLUDES
#include "libdli-api.h"

// EXTERNAL INCLUDES
#include "dali/public-api/common/vector-wrapper.h"
#include "dali/devel-api/common/map-wrapper.h"
#include <string>

namespace dli
{

///@brief Offers a description of an aspect of the watch face that can be customized:
///    the number of options, and the name of the nodes that are registered
///    for the tag, whose children will be shown / hidden based on selection.
struct LIBDLI_API Customization
{
  using Tag = std::string;
  using Map = std::map<Tag, Customization>;

  using OptionType = uint32_t;
  using Choices = std::map<Tag, OptionType>;

  static const int NONE = -1;

  OptionType numOptions = 0;
  std::vector<std::string> nodes;  // to apply option to.
};

}

#endif //LIBDLI_CUSTOMIZATION_STATE_H_

