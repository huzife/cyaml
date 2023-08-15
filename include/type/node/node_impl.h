/**
 * @file        node_impl.h
 * @brief       node 实现
 * @details     主要包含 YAML 的 Node 类模板函数实现
 * @date        2023-8-8
 */

#ifndef CYAML_NODE_IMPL_H
#define CYAML_NODE_IMPL_H

#include "type/node/node.h"
#include "type/node/convert.h"
#include <string>

namespace cyaml
{
    template<typename T>
    Node &Node::operator=(const T &rhs)
    {
        Node node = Convert<T>::encode(rhs);

        auto refs = data_->refs;
        for (auto *ref : refs) {
            ref->type_ = node.type_;
            ref->style_ = node.style_;
            ref->data_ = node.data_;
            node.data_->insert_ref(ref);
        }
        refs.clear();

        return *this;
    }

    template<typename T>
    T Node::as() const
    {
        T ret;
        if (Convert<T>::decode(*this, ret))
            return ret;

        throw Convertion_Exception();
    }

    template<typename T>
    bool Node::push_back(const T &rhs)
    {
        Node node = Convert<T>::encode(rhs);
        return push_back(node);
    }
} // namespace cyaml

#endif // CYAML_NODE_IMPL_H
