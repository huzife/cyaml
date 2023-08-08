/**
 * @file        node_impl.h
 * @brief       node 实现
 * @details     主要包含 YAML 的 Node 类模板函数实现
 * @date        2023-8-8
 */

#ifndef CYAML_NODE_IMPL_H
#define CYAML_NODE_IMPL_H

#include "type/node.h"
#include "type/convert.h"
#include <string>

namespace cyaml
{
    template<typename T>
    Node &Node::operator=(const T &rhs)
    {
        Node node = Convert<T>::encode(rhs);
        type_ = node.type_;
        data_ = node.data_;
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
} // namespace cyaml

#endif // CYAML_NODE_IMPL_H
