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

    void Serializer::fill_blank_to(Mark dest)
    {
        if (!on_begin()) {
            write_new_line(dest.line - mark_.line);
        }
        write_space(dest.column - mark_.column);
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
        if (node.is_map()) {
            write_map(node, indent);
        } else if (node.is_seq()) {
            write_seq(node, indent);
        } else if (node.is_scalar()) {
            write(node.scalar());
        }
    }

    void Serializer::write_map(const Node &node, uint32_t indent)
    {
        auto keys = node.keys();
        for (int i = 0; i < keys.size(); i++) {
            fill_blank_to(Mark(line() + 1, indent + 1));
            auto key = keys[i];
            if (key.is_map() || key.is_seq()) {
                write("? ");
                write_node(key, indent + indent_inc_);
                fill_blank_to(Mark(line() + 1, indent + 1));
            } else {
                write_node(key, indent + indent_inc_);
            }
            write(": ");
            write_node(node[key], indent + indent_inc_);
        }
    }

    void Serializer::write_seq(const Node &node, uint32_t indent)
    {
        for (int i = 0; i < node.size(); i++) {
            fill_blank_to(Mark(line() + 1, indent + 1));
            write("- ");
            write_node(node[i], indent + indent_inc_);
        }
    }

} // namespace cyaml
