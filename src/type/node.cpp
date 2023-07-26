/**
 * @file        node.cpp
 * @brief       YAML 数据节点，用于存储 YAML 类型数据
 * @details     主要包含 YAML 的 Node 类实现
 * @date        2023-7-26
 */

#include "type/node.h"

namespace cyaml
{
    Node::Node(Data_Type type): type_(type) {}

    Node::Node(std::string value)
        : type_(Data_Type::SCALAR),
          scalar_data_(value)
    {
    }

} // namespace cyaml
