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

    using namespace cyaml::type;

    // string
    template<>
    struct Convert<String>
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

            if (node.is_seq()) {
                bool first = true;
                rhs = "[";
                for (auto &i : node.seq_data_) {
                    if (first) {
                        first = false;
                    } else {
                        rhs += ", ";
                    }

                    std::string temp;
                    Convert<String>::decode(*i, temp);
                    rhs += temp;
                }
                rhs += "]";
                return true;
            }

            if (node.is_map()) {
                bool first = true;
                rhs = "{";
                for (auto &i : node.map_data_) {
                    if (first) {
                        first = false;
                    } else {
                        rhs += ", ";
                    }

                    rhs += i.first;
                    rhs += ": ";
                    std::string temp;
                    Convert<String>::decode(*(i.second), temp);
                    rhs += temp;
                }
                rhs += "}";
            }

            if (node.is_scalar())
                rhs = node.scalar_data_;

            return true;
        }
    };

    // int
    template<>
    struct Convert<Int>
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

    // real
    template<>
    struct Convert<Real>
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

    template<>
    struct Convert<Bool>
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

            std::string value = node.scalar_data_;
            if (value != "true" && value != "false")
                return false;

            rhs = value == "true" ? true : false;
            return true;
        }
    };

} // namespace cyaml

#endif // CYAML_CONVERT_H
