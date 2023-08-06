/**
 * @file        scalar_type.h
 * @brief       YAML 标量值类型
 * @details     声明 null, map, sequence, int, real, string 类型
 * @date        2023-8-1
 */

#ifndef CYAML_SCALAR_TYPE_H
#define CYAML_SCALAR_TYPE_H

#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

namespace cyaml
{
    class Node;
    using Node_Ptr = std::shared_ptr<Node>;

    namespace type
    {
        class Key_Map_Cmp
        {
        public:
            bool operator()(const Node_Ptr &n1, const Node_Ptr &n2) const
            {
                static bool ret = false;

                if (n1 == n2) {
                    return false;
                }

                ret = !ret;
                return ret;
            }
        };

        using Key_Map = std::map<Node_Ptr, std::string, Key_Map_Cmp>;
        using Map = std::unordered_map<std::string, Node_Ptr>;
        using Sequence = std::vector<Node_Ptr>;
        using Int = int;
        using Real = double;
        using String = std::string;
        using Bool = bool;

    } // namespace type

} // namespace cyaml

#endif // CYAML_SCALAR_TYPE_H
