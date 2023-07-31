/**
 * @file        node.h
 * @brief       YAML 数据节点，用于存储 YAML 类型数据
 * @details     主要包含 YAML 的 Node 类声明
 * @date        2023-7-25
 */

#ifndef CYAML_NODE_H
#define CYAML_NODE_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace cyaml
{
    class Node;
    using Node_Ptr = std::shared_ptr<Node>;
    using Mapping = std::unordered_map<std::string, Node_Ptr>;
    using Sequence = std::vector<Node_Ptr>;

    /**
     * @enum    Node_Type
     * @brief   声明节点的数据类型
     */
    enum class Node_Type
    {
        NULL_NODE,
        MAP,
        SEQUENCE,
        SCALAR
    };

    /**
     * @class   Node
     * @brief   YAML 数据节点
     */
    class Node: public std::enable_shared_from_this<Node>
    {
    public:
        Node_Type type_;
        Mapping map_data_;
        Sequence sequence_data_;
        std::string scalar_data_;

        /**
         * @brief   Node 类构造函数
         * @param   Node_Type       该节点数据类型
         * @retval  指定类型的节点
         */
        Node(Node_Type type);

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
        bool is_sequence() const
        {
            return type_ == Node_Type::SEQUENCE;
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
