/**
 * @file        convert.h
 * @brief       标量类型转换
 * @details     提供数据转换模板类 Convert，主要实现标量类型的转换
 * @date        2023-7-28
 */

#ifndef CYAML_CONVERT_H
#define CYAML_CONVERT_H

#include "type/node.h"
#include <string>

namespace cyaml
{
    /**
     * @struct  Convert
     * @brief   Node 与指定类型的转换模板类
     * @tparam  T   转换类型
     */
    template<typename T>
    struct Convert;

    // Node
    template<>
    struct Convert<Node>
    {
        static Node encode(const Node &rhs)
        {
            return rhs;
        }

        static bool decode(const Node &node, Node &rhs)
        {
            rhs = node;
            return true;
        }
    };

    // string
    template<>
    struct Convert<std::string>
    {
        static Node encode(const std::string &rhs)
        {
            return Node(rhs);
        }

        static bool decode(const Node &node, std::string &rhs)
        {
            if (node.is_null())
                return "null";

            if (!node.is_scalar())
                return false;

            rhs = node.scalar_data_;
            return true;
        }
    };

    // int
    template<>
    struct Convert<int>
    {
        static Node encode(const int &rhs)
        {
            return Node(std::to_string(rhs));
        }

        static bool decode(const Node &node, int &rhs)
        {
            rhs = std::stoi(node.scalar_data_);
            return true;
        }
    };

    // double
    template<>
    struct Convert<double>
    {
        static Node encode(const double &rhs)
        {
            return Node(std::to_string(rhs));
        }

        static bool decode(const Node &node, double &rhs)
        {
            rhs = std::stod(node.scalar_data_);
            return true;
        }
    };

} // namespace cyaml

#endif // CYAML_CONVERT_H
