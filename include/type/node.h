/**
 * @file        node.h
 * @brief       YAML 数据节点，用于存储 YAML 类型数据
 * @details     主要包含 YAML 的 Node 类声明
 * @date        2023-7-25
 */

#ifndef CYAML_NODE_H
#define CYAML_NODE_H

#include "type/type.h"

namespace cyaml
{
    /**
     * @enum    Node_Type
     * @brief   声明节点的数据类型
     */
    enum class Node_Type
    {
        NULL_NODE,
        MAP,
        SEQ,
        SCALAR
    };

    /**
     * @class   Node
     * @brief   YAML 数据节点
     */
    class Node: public std::enable_shared_from_this<Node>
    {
    public:
        Node_Type type_;          // 节点类型
        type::Map map_data_;      // 映射数据
        type::Sequence seq_data_; // 序列数据
        std::string scalar_data_; // 标量数据
        type::Node_List keys_;    // 键

        /**
         * @brief   Node 类构造函数，默认为空节点
         * @param   Node_Type       该节点数据类型
         * @retval  指定类型的节点
         */
        Node(Node_Type type = Node_Type::NULL_NODE);

        /**
         * @brief   Scalar Node 构造函数
         * @param   std::string     标量值
         * @retval  标量节点
         */
        Node(std::string value);

        /**
         * @brief   判断是否为 map
         * @return  bool
         */
        bool is_map() const
        {
            return type_ == Node_Type::MAP;
        }

        /**
         * @brief   判断是否为 sequence
         * @return  bool
         */
        bool is_seq() const
        {
            return type_ == Node_Type::SEQ;
        }

        /**
         * @brief   判断是否为 scalar
         * @return  bool
         */
        bool is_scalar() const
        {
            return type_ == Node_Type::SCALAR;
        }

        /**
         * @brief   判断是否为 null
         * @return  bool
         */
        bool is_null() const
        {
            return type_ == Node_Type::NULL_NODE;
        }

        Node(const Node &) = default;
        Node &operator=(const Node &) = default;
    };

} // namespace cyaml

#endif // CYAML_NODE_H
