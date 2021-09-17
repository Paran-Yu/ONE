/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include "selector.h"
#include "Read.h"

#include <iostream>

namespace selector
{
  
  void Operators::run(const circle::Model *model)
  {
    circleread::Reader reader(model);

    const uint32_t subgraph_size = reader.num_subgraph();

    for (uint32_t g=0; g < subgraph_size; g++)
    {
      reader.select_subgraph(g);
      auto ops = reader.operators();

      for (uint32_t i = 0; i < ops->Length(); ++i)
      {
        const auto op = ops->Get(i);
        auto op_name = reader.opcode_name(op);
        std::cout << op_name << std::endl;
      }
    }

    // create flatbuffer builder
    auto flatbuffer_builder = std::make_unique<flatbuffers::FlatBufferBuilder>(1024);
    // create ModelBuilder
    circle::ModelBuilder model_builder{*flatbuffer_builder};
    
    auto new_version = model->version();
    auto new_description = model->description();
    auto new_operator_codes = model->operator_codes();
    auto new_subgraphs = model->subgraphs();
    auto new_buffers = model->buffers();
    auto new_metadata_buffer = model->metadata_buffer();
    auto new_metadata = model->metadata();

    model_builder.add_version(new_version);
    model_builder.add_description(new_description);
    model_builder.add_operator_codes(new_operator_codes);
    model_builder.add_subgraphs(new_subgraphs);
    model_builder.add_buffers(new_buffers);
    model_builder.add_metadata_buffer(new_metadata_buffer);
    model_builder.add_metadata(new_metadata);

    auto new_model = model_builder.Finish();
    circle::FinishModelBuffer(flatbuffer_builder, new_model);
    
  }

  


} // namespace selector
