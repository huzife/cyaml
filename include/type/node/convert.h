/**
 * @file        convert.h
 * @brief       标量类型转换
 * @details     提供数据转换模板类 Convert，主要实现标量类型的转换
 * @date        2023-7-28
 */

#ifndef CYAML_CONVERT_H
#define CYAML_CONVERT_H

#include "type/node/node.h"
#include <string>
#include <iostream>
#include <assert.h>

namespace cyaml
{
    /**
     * @struct  Converter
     * @brief   Node 与指定类型的转换模板类
     * @tparam  T   转换类型
     */
    template<typename T>
    struct Converter;

    // Node
    template<>
    struct Converter<Node>
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
    struct Converter<std::string>
    {
        static Node encode(const std::string &rhs)
        {
            return Node(rhs);
        }

        static bool decode(const Node &node, std::string &rhs)
        {
            if (node.is_null()) {
                rhs = "null";
                return true;
            }

            if (!node.is_scalar())
                return false;

            rhs = node.scalar();
            return true;
        }
    };

    // const char *
    template<>
    struct Converter<const char *>
    {
        static Node encode(const char *rhs)
        {
            return Node(rhs);
        }
    };

    // char *
    template<>
    struct Converter<char *>
    {
        static Node encode(const char *rhs)
        {
            return Node(rhs);
        }
    };

    // char[]
    template<std::size_t N>
    struct Converter<char[N]>
    {
        static Node encode(const char *rhs)
        {
            return Node(rhs);
        }
    };

    // int
    template<>
    struct Converter<int>
    {
        static Node encode(const int &rhs)
        {
            return Node(std::to_string(rhs));
        }

        static bool decode(const Node &node, int &rhs)
        {
            rhs = std::stoi(node.scalar());
            return true;
        }
    };

    // float
    template<>
    struct Converter<float>
    {
        static Node encode(const float &rhs)
        {
            return Node(std::to_string(rhs));
        }

        static bool decode(const Node &node, float &rhs)
        {
            rhs = std::stof(node.scalar());
            return true;
        }
    };

    template<>
    struct Converter<bool>
    {
        static Node encode(const bool &rhs)
        {
            if (rhs)
                return Node("true");
            else
                return Node("false");
        }

        static bool decode(const Node &node, bool &rhs)
        {
            if (!node.is_scalar())
                return false;

            std::string value = node.scalar();
            if (value != "true" && value != "false")
                return false;

            rhs = value == "true" ? true : false;
            return true;
        }
    };

} // namespace cyaml

#endif // CYAML_CONVERT_H
