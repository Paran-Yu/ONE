/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd. All Rights Reserved
 * Copyright 2019 The TensorFlow Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ONERT_BACKEND_GPU_CL_OPEN_CL_KERNELS_SOFTMAX_H__
#define __ONERT_BACKEND_GPU_CL_OPEN_CL_KERNELS_SOFTMAX_H__

#include "open_cl/ClKernel.h"
#include "GpuOperation.h"
#include "open_cl/Precision.h"
#include "open_cl/Tensor.h"
#include "open_cl/Types.h"

namespace onert
{
namespace backend
{
namespace gpu_cl
{

GPUOperation CreateSoftmax(const OperationDef &definition);

} // namespace gpu_cl
} // namespace backend
} // namespace onert

#endif // __ONERT_BACKEND_GPU_CL_OPEN_CL_KERNELS_SOFTMAX_H__
