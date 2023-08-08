/**
 * @file        node.cpp
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Node 类实现
 * @date        2023-7-26
 */

#include "type/node.h"
#include "error/exceptions.h"
#include <iostream>

namespace cyaml
{
    Node::Node(): type_(Node_Type::NULL_NODE) {}

    Node::Node(Node_Type type)
        : type_(type),
          data_(std::make_shared<Node_Data>())
    {
    }

    Node::Node(const Node_Ptr &node): data_(node->data_) {}

    Node::Node(Node_Type type, const Node_Data_Ptr &data)
        : type_(type),
          data_(data)
    {
    }

    Node::Node(const std::string &scalar)
        : type_(Node_Type::SCALAR),
          data_(std::make_shared<Node_Data>(scalar))
    {
    }

    // Node::Node(const Node &node): type_(node.type_), data_(node.data_) {}

    uint32_t Node::size() const
    {
        switch (type_) {
        case Node_Type::NULL_NODE:
            return 0;
        case Node_Type::MAP:
            return map().size();
        case Node_Type::SEQ:
            return seq().size();
        case Node_Type::SCALAR:
            return scalar().size();
        }

        return 0;
    }

    Node Node::operator[](uint32_t index)
    {
        if (!is_seq())
            throw Dereference_Exception();

        ///< @todo  out_of_range exception
        if (index >= seq().size())
            throw Dereference_Exception();

        return *(seq()[index]);
    }

    Node Node::operator[](const std::string &key)
    {
        if (!is_map())
            throw Dereference_Exception();

        ///< @todo  key_not_found exception
        if (!find(key))
            throw Dereference_Exception();

        return *(map()[key]);
    }

    Node Node::operator[](const Node &key)
    {
        if (!is_map())
            throw Dereference_Exception();

        ///< @todo  key_not_found exception
        if (!find(key))
            throw Dereference_Exception();

        return *(map()[key.as<std::string>()]);
    }

    Node &Node::operator=(const Node &rhs)
    {
        type_ = rhs.type();
        data_ = rhs.data_;

        ///< @todo  处理 yaml 引用

        return *this;
    }

    bool Node::find(std::string key) const
    {
        if (!is_map())
            return false;

        return map().find(key) != map().end();
    }

    bool Node::find(const Node &value) const
    {
        if (!is_map())
            return false;

        std::string key = value.as<std::string>();
        return map().find(key) != map().end();
    }

    bool Node::insert(const Node &key, const Node &value)
    {
        if (!is_map())
            return false;

        data_->keys().emplace_back(std::make_shared<Node>(key));
        data_->map()[key.as<std::string>()] = std::make_shared<Node>(value);
        return true;
    }

    bool Node::push_back(const Node &node)
    {
        if (!is_seq())
            return false;

        data_->seq().emplace_back(std::make_shared<Node>(node));
        return true;
    }

} // namespace cyaml
