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

    class Node_Data;
    using Node_Data_Ptr = std::shared_ptr<Node_Data>;

    namespace type
    {
        using Node_List = std::vector<Node_Ptr>;
        using Map = std::unordered_map<std::string, Node_Ptr>;
        using Sequence = std::vector<Node_Ptr>;

    } // namespace type

} // namespace cyaml

namespace cyaml
{

    /**
     * @class   Node
     * @brief   YAML 数据节点
     */
    class Node_Data
    {
    private:
        type::Map map_data_;      // 映射数据
        type::Sequence seq_data_; // 序列数据
        std::string scalar_data_; // 标量数据
        type::Node_List keys_;    // 键

    public:
        Node_Data() = default;
        Node_Data(std::string value);
        Node_Data(const Node_Data &) = default;
        Node_Data &operator=(const Node_Data &) = default;

        // 访问函数
        type::Map &map()
        {
            return map_data_;
        }

        type::Sequence &seq()
        {
            return seq_data_;
        }

        std::string &scalar()
        {
            return scalar_data_;
        }

        type::Node_List &keys()
        {
            return keys_;
        }
    };

} // namespace cyaml

#endif // CYAML_NODE_H
