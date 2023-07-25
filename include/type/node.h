/**
 * @file        node.h
 * @brief       YAML 数据节点，用于存储 YAML 类型数据
 * @details     主要包含 YAML 的 Node 类定义
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
    /**
     * @enum    Data_Type
     * @brief   定义节点的数据类型
     */
    enum class Data_Type
    {
        MAPPING,
        SEQUENCE,
        SCALAR
    };

    /**
     * @class   Node
     * @brief   YAML 数据节点
     */
    class Node
    {
    public:
        using Ptr = std::shared_ptr<Node>;

    private:
        Data_Type type_;
        std::unordered_map<std::string, Ptr> mapping_data_;
        std::vector<Ptr> sequence_data_;
        std::string scalar_data_;

    public:
        /**
         * @brief   Node 类无参构造函数
         * @retval  Node 对象，数据类型为 scalar null
         */
        Node();
    };

} // namespace cyaml

#endif // CYAML_NODE_H
