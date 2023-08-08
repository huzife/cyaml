/**
 * @file        convert.h
 * @brief       标量类型转换
 * @details     提供数据转换模板类 Convert，主要实现标量类型的转换
 * @date        2023-7-28
 */

#ifndef CYAML_CONVERT_H
#define CYAML_CONVERT_H

#include "type/node_data.h"
#include <string>
#include <iostream>

namespace cyaml
{
    /**
     * @struct  Convert
     * @brief   Node_Data 与指定类型的转换模板类
     * @tparam  T   转换类型
     */
    template<typename T>
    struct Convert;

    // Node_Data
    template<>
    struct Convert<Node_Data>
    {
        static Node_Data encode(const Node_Data &rhs)
        {
            return rhs;
        }

        static bool decode(const Node_Data &node, Node_Data &rhs)
        {
            rhs = node;
            return true;
        }
    };

    using namespace cyaml::type;

    // string
    template<>
    struct Convert<std::string>
    {
        static Node_Data encode(const std::string &rhs)
        {
            return Node_Data(rhs);
        }

        static bool decode(const Node_Data &node, std::string &rhs)
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
                    Convert<std::string>::decode(*i, temp);
                    rhs += temp;
                }
                rhs += "]";
                return true;
            }

            if (node.is_map()) {
                bool first = true;
                rhs = "{";
                for (auto &i : node.keys_) {
                    if (first) {
                        first = false;
                    } else {
                        rhs += ", ";
                    }

                    std::string key;
                    std::string value;
                    Convert<std::string>::decode(*i, key);
                    Convert<std::string>::decode(
                            *(node.map_data_.find(key)->second), value);
                    rhs += key;
                    rhs += ": ";
                    rhs += value;
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
    struct Convert<int>
    {
        static Node_Data encode(const int &rhs)
        {
            return Node_Data(std::to_string(rhs));
        }

        static bool decode(const Node_Data &node, int &rhs)
        {
            rhs = std::stoi(node.scalar_data_);
            return true;
        }
    };

    // float
    template<>
    struct Convert<float>
    {
        static Node_Data encode(const float &rhs)
        {
            return Node_Data(std::to_string(rhs));
        }

        static bool decode(const Node_Data &node, float &rhs)
        {
            rhs = std::stof(node.scalar_data_);
            return true;
        }
    };

    template<>
    struct Convert<bool>
    {
        static Node_Data encode(const bool &rhs)
        {
            if (rhs)
                return Node_Data("true");
            else
                return Node_Data("false");
        }

        static bool decode(const Node_Data &node, bool &rhs)
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
