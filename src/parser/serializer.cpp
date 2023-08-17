/**
 * @file        serializer.cpp
 * @brief       YAML 序列化部分 Serializer 类源文件
 * @details     主要包含 YAML 的 Serializer 类定义
 * @date        2023-8-10
 */

#include "parser/serializer.h"
#include <algorithm>
#include <unistd.h>

namespace cyaml
{
    Serializer::Serializer(std::ostream &out): output_stream_(out) {}

    Serializer &Serializer::operator<<(const Node &node)
    {
        write_node(node, 0);
        return *this;
    }

    void Serializer::fill_blank(uint32_t indent)
    {
        if (column() < indent + 1) {
            write_space(indent + 1 - column());
        }
    }

    void Serializer::write(std::string str)
    {
        output_stream_ << str;

        // 计算位置
        int line_cnt = std::count(str.begin(), str.end(), '\n');
        int column_cnt;

        if (line_cnt == 0) {
            column_cnt = str.size();
        } else {
            mark_.column = 1;
            column_cnt = str.size() - 1 - str.find_last_of('\n');
        }

        mark_.line += line_cnt;
        mark_.column += column_cnt;
    }

    void Serializer::write_space(int count)
    {
        assert(count >= 0);
        write(std::string(count, ' '));
    }

    void Serializer::write_new_line(int count)
    {
        assert(count >= 0);
        write(std::string(count, '\n'));
    }

    void Serializer::write_node(const Node &node, uint32_t indent)
    {
        if (node.style() == Node_Style::BLOCK) {
            write_block_node(node, indent);
        } else {
            write_flow_node(node);
        }
    }

    void Serializer::write_block_node(const Node &node, uint32_t indent)
    {
        if (node.is_null()) {
            write("null");
        } else if (node.is_map()) {
            write_block_map(node, indent);
        } else if (node.is_seq()) {
            write_block_seq(node, indent);
        } else if (node.is_scalar()) {
            std::string str = node.scalar();
            if (str.empty() || str == "~" || str == "null") {
                str = '"' + str + '"';
            }
            write(str);
        }
    }

    void Serializer::write_flow_node(const Node &node)
    {
        if (node.is_null()) {
            write("null");
        } else if (node.is_map()) {
            write_flow_map(node);
        } else if (node.is_seq()) {
            write_flow_seq(node);
        } else if (node.is_scalar()) {
            std::string str = node.scalar();
            if (str.empty() || str == "~" || str == "null") {
                str = '"' + str + '"';
            }
            write(str);
        }
    }

    void Serializer::write_block_map(const Node &node, uint32_t indent)
    {
        for (auto &i : node.keys()) {
            write_key(i, indent);
            write_value(node[i], indent);
        }
    }

    void Serializer::write_block_seq(const Node &node, uint32_t indent)
    {
        for (int i = 0; i < node.size(); i++) {
            fill_blank(indent);
            write("- ");
            if (!line_style(node[i])) {
                write_new_line();
                write_node(node[i], increase(indent));
            } else {
                write_node(node[i], increase(indent));
                write_new_line();
            }
        }
    }

    void Serializer::write_flow_map(const Node &node)
    {
        write("{");
        bool first = true;
        for (auto &i : node.keys()) {
            if (first) {
                first = false;
            } else {
                write(", ");
            }
            write_flow_node(i);
            write(": ");
            write_flow_node(i);
        }
        write("}");
    }

    void Serializer::write_flow_seq(const Node &node)
    {
        write("[");
        for (int i = 0; i < node.size(); i++) {
            if (i > 0) {
                write(", ");
            }
            write_flow_node(node[i]);
        }
        write("]");
    }

    void Serializer::write_key(const Node &node, uint32_t indent)
    {
        fill_blank(indent);
        if (!line_style(node)) {
            write("? ");
        }
        write_node(node, increase(indent));
    }

    void Serializer::write_value(const Node &node, uint32_t indent)
    {
        fill_blank(indent);
        write(": ");
        if (column() > increase(indent) + 1 && !line_style(node)) {
            write_new_line();
        }
        write_node(node, increase(indent));
        if (line_style(node)) {
            write_new_line();
        }
    }

} // namespace cyaml
