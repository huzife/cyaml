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
        size_t ret = 0;

        if (node.is_null()) {
            ret = std::hash<size_t>()(0);
        } else if (node.is_map()) {
            for (auto &key : node.data_->keys) {
                ret ^= (node_hash(key) ^ node_hash(node.data_->map[key]));
            }
        } else if (node.is_seq()) {
            for (auto &i : node.data_->seq) {
                ret ^= node_hash(i);
            }
        }
        else if (node.is_scalar()) {
            ret = std::hash<std::string>()(node.scalar());
        }

        return ret;
    }

    Node_Data::Node_Data(std::string value): scalar(value) {}

} // namespace cyaml
