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
#include <unordered_set>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <iostream>

// 类型声明
namespace cyaml
{
    class Node;
    using Node_Ptr = std::shared_ptr<Node>;
    using Ref_Node_Ptr = std::weak_ptr<Node>;

    class Node_Data;
    using Node_Data_Ptr = std::shared_ptr<Node_Data>;

    using KV_Pair = std::pair<Node_Ptr, Node_Ptr>;
    using Map = std::list<KV_Pair>;
    using Sequence = std::vector<Node_Ptr>;

    class Ref_Hash
    {
    public:
        size_t operator()(const Ref_Node_Ptr &node) const
        {
            return std::hash<Node_Ptr>()(node.lock());
        }
    };

} // namespace cyaml

namespace cyaml
{
    /**
     * @struct  Node_Data
     * @brief   YAML 数据节点
     */
    struct Node_Data
    {
        Map map;            // 映射数据
        Sequence seq;       // 序列数据
        std::string scalar; // 标量数据

        std::unordered_set<Node *> refs; // 引用的节点

        Node_Data() = default;
        Node_Data(std::string value);
        Node_Data(const Node_Data &) = default;
        Node_Data &operator=(const Node_Data &) = default;

        void insert_ref(const Node *node);
        void remove_ref(const Node *node);
    };

} // namespace cyaml

#endif // CYAML_NODE_H
