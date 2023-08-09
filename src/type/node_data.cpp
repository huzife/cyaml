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
    size_t node_hash(const Node_Ptr &node)
    {
        return node_hash(*node);
    }

    size_t node_hash(const Node &node)
    {
        if (node.is_scalar()) {
            return std::hash<std::string>()(node.scalar());
        }

        return std::hash<Node_Data_Ptr>()(node.data_);
    }

    Node_Data::Node_Data(std::string value): scalar(value) {}

} // namespace cyaml
