#ifndef LIBDLI_MATRIX_STACK_H_
#define LIBDLI_MATRIX_STACK_H_
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
#include "dali/public-api/math/matrix.h"
#include "dali/public-api/common/vector-wrapper.h"

namespace dli
{

///@brief A stack of matrices whereby each newly pushed matrix is stored
/// after being multiplied by the previous one (if any).
///@note Current implementation reserves space for 16 matrices.
class LIBDLI_API MatrixStack
{
public:
  MatrixStack();

  void Push(const Dali::Matrix& model);
  const Dali::Matrix& Top() const;
  void Pop();
  void PopAll();  // clears the stack, but retains the storage.

private:
  std::vector<Dali::Matrix> mModel;
};

}

#endif //LIBDLI_MATRIX_STACK_H_
