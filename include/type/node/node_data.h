/**
 * @file        node_data.h
 * @brief       Node_Data 类，用于存储 YAML 数据
 * @details     主要包含 YAML 的 Node_Data 类声明
 * @date        2023-7-25
 */

#ifndef CYAML_NODE_H
#define CYAML_NODE_H

#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

// 类型声明
namespace cyaml
{
    class Node;
    using Node_Ptr = std::shared_ptr<Node>;
    using P_Node_Ptr = std::weak_ptr<Node>;

    class Node_Data;
    using Node_Data_Ptr = std::shared_ptr<Node_Data>;

    using Node_Hash = size_t (*)(const Node_Ptr &);
    size_t node_hash(const Node_Ptr &node);
    size_t node_hash(const Node &node);

    namespace type
    {
        using Node_List = std::vector<Node_Ptr>;
        using Map = std::unordered_map<Node_Ptr, Node_Ptr, Node_Hash>;
        using Sequence = std::vector<Node_Ptr>;

    } // namespace type

} // namespace cyaml

namespace cyaml
{
    /**
     * @struct  Node_Data
     * @brief   YAML 数据节点
     */
    struct Node_Data
    {
        type::Map map = type::Map(0, node_hash); // 映射数据
        type::Sequence seq;                      // 序列数据
        std::string scalar;                      // 标量数据
        type::Node_List keys; // 键(用于实现顺序读取)

        Node_Data() = default;
        Node_Data(std::string value);
        Node_Data(const Node_Data &) = default;
        Node_Data &operator=(const Node_Data &) = default;
    };

} // namespace cyaml

#endif // CYAML_NODE_H
