/**
 * @file        node_data.cpp
 * @brief       YAML 数据节点，用于存储 YAML 类型数据
 * @details     主要包含 YAML 的 Node_Data 类实现
 * @date        2023-7-26
 */

#include "type/node/node_data.h"
#include "type/node/node.h"

namespace cyaml
{
    Node_Data::Node_Data(std::string value): scalar(value) {}

} // namespace cyaml
