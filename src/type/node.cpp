/**
 * @file        node.cpp
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Node 类实现
 * @date        2023-7-26
 */

#include "type/node/node.h"
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

    Node::Node(const Node_Ptr &node): type_(node->type_), data_(node->data_) {}

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

    bool operator==(const Node &n1, const Node &n2)
    {
        if (n1.is_null() && n2.is_null())
            return true;

        if (n1.type() != n2.type() || n1.size() != n2.size())
            return false;

        if (n1.type() == n2.type() && n1.data_ == n2.data_)
            return true;

        if (n1.is_scalar() && n2.is_scalar())
            return (n1.scalar() == n2.scalar());

        if (n1.is_map() && n2.is_map()) {
            if (n1.size() != n2.size())
                return false;

            for (int i = 0; i < n1.size(); i++) {
                auto key1 = n1.data_->keys[i];
                auto key2 = n2.data_->keys[i];
                if (key1 != key2 || n1.data_->map[key1] != n2.data_->map[key2])
                    return false;
            }

            return true;
        }

        if (n1.is_seq() && n2.is_seq()) {
            if (n1.size() != n2.size())
                return false;

            for (int i = 0; i < n1.size(); i++) {
                if (n1.data_->seq[i] != n2.data_->seq[i])
                    return false;
            }

            return true;
        }

        return false;
    }

    bool operator==(const Node_Ptr &n1, const Node_Ptr &n2)
    {
        return (*n1 == *n2);
    }

    bool operator!=(const Node &n1, const Node &n2)
    {
        return !(n1 == n2);
    }

    bool operator!=(const Node_Ptr &n1, const Node_Ptr &n2)
    {
        return !(n1 == n2);
    }

    std::ostream &operator<<(std::ostream &out, const Node &node)
    {
        if (node.is_null() || node.is_scalar()) {
            out << node.scalar();
        } else if (node.is_map()) {
            for (auto &key : node.keys()) {
                bool complex = key->is_map() || key->is_seq();
                if (complex) {
                    out << "? " << std::endl;
                }
                out << *key;
                out << ": ";
                out << *node.map().find(key)->second;
                out << std::endl;
            }
        } else if (node.is_seq()) {
            for (auto &i : node.seq()) {
                out << "- " << *i;
                out << std::endl;
            }
        }
        return out;
    }

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

    Node &Node::operator[](uint32_t index)
    {
        if (is_null()) {
            reset(Node_Type::SEQ);
        }

        if (!is_seq())
            throw Dereference_Exception();

        ///< @todo  out_of_range exception
        if (index >= data_->seq.size())
            throw Dereference_Exception();

        return *(data_->seq[index]);
    }

    Node &Node::operator[](const std::string &key)
    {
        if (is_null()) {
            reset(Node_Type::MAP);
        }

        if (!is_map())
            throw Dereference_Exception();

        auto key_node = std::make_shared<Node>(key);
        if (!find(key)) {
            insert(key, Node());
        }

        return *(data_->map[key_node]);
    }

    Node &Node::operator[](const Node &key)
    {
        if (is_null()) {
            reset(Node_Type::MAP);
        }

        if (!is_map())
            throw Dereference_Exception();

        auto key_node = std::make_shared<Node>(key);
        if (!find(key)) {
            insert(key, Node());
        }

        return *(data_->map[key_node]);
    }

    Node &Node::operator=(const Node &rhs)
    {
        type_ = rhs.type();
        data_ = rhs.data_;

        return *this;
    }

    bool Node::find(std::string key) const
    {
        if (!is_map())
            return false;

        auto key_node = std::make_shared<Node>(key);
        return data_->map.find(key_node) != data_->map.end();
    }

    bool Node::find(const Node &key) const
    {
        if (!is_map())
            return false;

        auto key_node = std::make_shared<Node>(key);
        return data_->map.find(key_node) != data_->map.end();
    }

    bool Node::insert(const Node &key, const Node &value)
    {
        if (is_null()) {
            reset(Node_Type::MAP);
        }

        if (!is_map())
            return false;

        // 插入节点
        auto key_node = std::make_shared<Node>(key);
        auto value_node = std::make_shared<Node>(value);
        data_->keys.emplace_back(key_node);
        data_->map[key_node] = value_node;

        return true;
    }

    bool Node::push_back(const Node &node)
    {
        if (is_null()) {
            reset(Node_Type::SEQ);
        }

        if (!is_seq())
            return false;

        data_->seq.emplace_back(std::make_shared<Node>(node));

        return true;
    }

} // namespace cyaml
