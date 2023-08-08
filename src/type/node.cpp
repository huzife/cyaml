/**
 * @file        node.cpp
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Node 类实现
 * @date        2023-7-26
 */

#include "type/node.h"
#include "error/exceptions.h"

namespace cyaml
{
    Node::Node(Node_Data_Ptr &node): node_(node) {}

    Node Node::operator[](uint32_t index)
    {
        if (!node_->is_seq())
            throw Dereference_Exception();

        ///< @todo  out_of_range exception
        if (index >= node_->seq_data_.size())
            throw Dereference_Exception();

        return Node(node_->seq_data_[index]);
    }

    Node Node::operator[](const std::string &key)
    {
        if (!node_->is_map())
            throw Dereference_Exception();

        ///< @todo  key_not_found exception
        if (!find(key))
            throw Dereference_Exception();

        return Node(node_->map_data_[key]);
    }

    Node Node::operator[](const Node &key)
    {
        if (!node_->is_map())
            throw Dereference_Exception();

        ///< @todo  key_not_found exception
        if (!find(key))
            throw Dereference_Exception();

        return Node(node_->map_data_[key.as<std::string>()]);
    }

    Node &Node::operator=(const Node &rhs)
    {
        node_ = rhs.node_;

        ///< @todo  处理 yaml 引用

        return *this;
    }

    bool Node::find(std::string key) const
    {
        if (!is_map())
            return false;

        return node_->map_data_.find(key) != node_->map_data_.end();
    }

    bool Node::find(const Node &value) const
    {
        if (!is_map())
            return false;

        std::string key = value.as<std::string>();
        return node_->map_data_.find(key) != node_->map_data_.end();
    }

    uint32_t Node::size() const
    {
        switch (node_->type_) {
        case Node_Type::MAP:
            return node_->map_data_.size();
        case Node_Type::SEQ:
            return node_->seq_data_.size();
        case Node_Type::SCALAR:
            return node_->scalar_data_.size();
        }

        return 0;
    }

} // namespace cyaml
