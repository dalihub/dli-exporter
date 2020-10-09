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
#include "libdli/matrix-stack.h"
#include "libdli/utils.h"

using namespace Dali;

namespace dli
{

MatrixStack::MatrixStack()
{
  mModel.reserve(16);
}

void MatrixStack::Push(const Dali::Matrix& model)
{
  if (mModel.empty())
  {
    mModel.push_back(model);
  }
  else
  {
    Matrix m{ false };
    Matrix::Multiply(m, model, mModel.back());
    mModel.push_back(m);
  }
}

const Dali::Matrix& MatrixStack::Top() const
{
  return mModel.back();
}

void MatrixStack::Pop()
{
  DALI_ASSERT_ALWAYS(mModel.size() > 0);
  mModel.pop_back();
}

void MatrixStack::PopAll()
{
  mModel.clear();
}

}
