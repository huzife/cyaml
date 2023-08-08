/**
 * @file        node_data.cpp
 * @brief       YAML 数据节点，用于存储 YAML 类型数据
 * @details     主要包含 YAML 的 Node_Data 类实现
 * @date        2023-7-26
 */

#include "type/node_data.h"

namespace cyaml
{
    Node_Data::Node_Data(Node_Type type): type_(type) {}

    Node_Data::Node_Data(std::string value)
        : type_(Node_Type::SCALAR),
          scalar_data_(value)
    {
    }

} // namespace cyaml