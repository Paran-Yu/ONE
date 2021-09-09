/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include "OpSelector.h"
// TODO Add new pass headers

#include <foder/FileLoader.h>

#include <luci/Importer.h>
#include <luci/CircleExporter.h>
#include <luci/CircleFileExpContract.h>

#include <arser/arser.h>
#include <vconone/vconone.h>

#include <iostream>
#include <string>
#include <vector>

void print_version(void)
{
  std::cout << "circle-opselector version " << vconone::get_string() << std::endl;
  std::cout << vconone::get_copyright() << std::endl;
}

bool check_input(std::string str)
{
  bool check_hyphen = false;

  for (char c : str)
  {
    if ('0' <= c && c <= '9')
      continue;
    else if (check_hyphen) // when user enter '-' more than 2.
    {
      std::cout << "Too many '-' in str." << std::endl;
      exit(0);
    }
    else if (c == '-')
      check_hyphen = true;
    else // when user enter not allowed character, print alert msg.
    {
      std::cout << "To select operator by id, please use these args: [0-9], '-', ','" << std::endl;
      exit(0);
    }
  }
  return true;
}

void split_id_input(const std::string &str, std::vector<int> &by_id)
{
  std::istringstream ss;
  ss.str(str);
  std::string str_buf;

  while (getline(ss, str_buf, ','))
  {
    if (str_buf.length() && check_input(str_buf)) // input validation
    {
      if (str_buf.find('-') == std::string::npos) // if token has no '-'
        by_id.push_back(stoi(str_buf));
      else // tokenize again by '-'
      {
        std::istringstream ss2(str_buf);
        std::string token;
        int from_to[2], top = 0;

        while (getline(ss2, token, '-'))
          from_to[top++] = stoi(token);

        for (int number = from_to[0]; number <= from_to[1]; number++)
          by_id.push_back(number);
      }
    }
  }
}

void split_name_input(const std::string &str, std::vector<std::string> &by_name)
{
  std::istringstream ss;
  ss.str(str);
  std::string str_buf;

  while (getline(ss, str_buf, ','))
    by_name.push_back(str_buf);
}

int entry(int argc, char **argv)
{
  // TODO Add new option names!

  arser::Arser arser("circle-opselector provides selecting operations in circle model");

  arser.add_argument("--version")
    .nargs(0)
    .required(false)
    .default_value(false)
    .help("Show version information and exit")
    .exit_with(print_version);

  // TODO Add new options!

  arser.add_argument("--input").nargs(1).type(arser::DataType::STR).help("Input circle model");
  arser.add_argument("--output").nargs(1).type(arser::DataType::STR).help("Output circle model");

  // select option
  arser.add_argument("--by_id")
    .nargs(1)
    .type(arser::DataType::STR)
    .help("Input operation id to select nodes.");
  arser.add_argument("--by_name")
    .nargs(1)
    .type(arser::DataType::STR)
    .help("Input operation name to select nodes.");

  try
  {
    arser.parse(argc, argv);
  }
  catch (const std::runtime_error &err)
  {
    std::cout << err.what() << std::endl;
    std::cout << arser;
    return EXIT_FAILURE;
  }

  std::string input_path = arser.get<std::string>("--input");
  std::string output_path = arser.get<std::string>("--output");

  std::string operator_input;

  std::vector<int> by_id;
  std::vector<std::string> by_name;

  if (arser["--by_id"])
  {
    operator_input = arser.get<std::string>("--by_id");
    split_id_input(operator_input, by_id);
  }
  if (arser["--by_name"])
  {
    operator_input = arser.get<std::string>("--by_name");
    split_name_input(operator_input, by_name);
  }

  // option parsing test code.
  for (int x : by_id)
    std::cout << "by_id: " << x << std::endl;

  for (std::string line : by_name)
    std::cout << "by_name: " << line << std::endl;

  // Load model from the file
  foder::FileLoader file_loader{input_path};
  std::vector<char> model_data = file_loader.load();

  // Verify flatbuffers
  flatbuffers::Verifier verifier{reinterpret_cast<uint8_t *>(model_data.data()), model_data.size()};
  if (!circle::VerifyModelBuffer(verifier))
  {
    std::cerr << "ERROR: Invalid input file '" << input_path << "'" << std::endl;
    return EXIT_FAILURE;
  }

  const circle::Model *circle_model = circle::GetModel(model_data.data());
  if (circle_model == nullptr)
  {
    std::cerr << "ERROR: Failed to load circle '" << input_path << "'" << std::endl;
    return EXIT_FAILURE;
  }

  // Import from input Circle file
  luci::Importer importer;
  auto module = importer.importModule(circle_model);

  // TODO Add function
  if (by_id.size())
  {
    std::cout<<"input: "<<module.get()->graph()->inputs()->at(0)->name()<<std::endl;
    std::cout<<"module.size(number of subgraph): "<<module.get()->size()<<std::endl;
    std::cout<<"module.graph.nodes.size(number of operator): "<<module.get()->graph()->nodes()->size()<<std::endl;
    std::map<uint32_t, std::string> _source_table = module.get()->source_table();
    /*std::cout<<"map Content::"<<std::endl;
    for(int i=2; i<8; i++)
    {
      _source_table.erase(i);
    }
    module.get()->source_table(_source_table);
     _source_table=module.get()->source_table();*/
    for(auto iter=_source_table.begin();iter!=_source_table.end();iter++)
      std::cout<<iter->first<<" "<<iter->second<<std::endl;
  }
  if (by_name.size())
  {
    opselector::OpSelector *selector = new opselector::OpSelector();
    std::map<uint32_t, std::string> _source_table = module.get()->source_table();
    std::vector<std::string> named_output;
    for(auto name : by_name) // find
    {
      for(auto iter=_source_table.begin();iter!=_source_table.end();iter++)
        if(iter->second.find(name)!= std::string::npos)
            named_output.push_back(iter->second);
    }
    
    selector->select_nodes_by_name(module.get(), named_output);
  }

  // Export to output Circle file
  luci::CircleExporter exporter;

  luci::CircleFileExpContract contract(module.get(), output_path);

  if (!exporter.invoke(&contract))
  {
    std::cerr << "ERROR: Failed to export '" << output_path << "'" << std::endl;
    return EXIT_FAILURE;
  }

  return 0;
}