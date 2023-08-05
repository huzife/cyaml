/**
 * @file        scan_token.cpp
 * @brief       token 解析
 * @details     主要包含 Scanner 中扫描并识别各种 token 的实现
 * @date        2023-7-31
 */

#include "parser/scanner.h"
#include "error/exceptions.h"
#include <iostream>

namespace cyaml
{
    void Scanner::stream_end()
    {
        // 匹配剩下的缩进
        pop_all_indent();

        // 添加 STREAM_END
        add_token(Token_Type::STREAM_END);
    }

    void Scanner::scan_doc_start()
    {
        for (auto i = 0; i < 3; i++) {
            next_char();
        }

        add_token(Token_Type::DOC_START);
    }

    void Scanner::scan_doc_end()
    {
        pop_all_indent();

        for (int i = 0; i < 3; i++) {
            next_char();
        }

        add_token(Token_Type::DOC_END);
    }

    void Scanner::scan_block_entry()
    {
        next_char();
        push_indent(Indent_Type::SEQ);
        pop_indent();
        start_scalar();
        add_token(Token_Type::BLOCK_ENTRY);
    }

    void Scanner::scan_key()
    {
        next_char();
        push_indent(Indent_Type::MAP);
        pop_indent();
        start_scalar();
        add_token(Token_Type::KEY);
    }

    void Scanner::scan_value()
    {
        next_char();

        // 处理复杂 key 的缩进，普通 key 不处理
        if (in_block()) {
            if (cur_indent_ < indent_.top().len) {
                pop_indent();
            }

            // 复杂结构的 VALUE 后要设置标量缩进，以及处理 KEY 的缩进
            if (cur_indent_ == indent_.top().len) {
                start_scalar();
            }
        }

        add_token(Token_Type::VALUE);
    }

    void Scanner::scan_flow_start()
    {
        Flow_Type type = next_char() == '{' ? Flow_Type::MAP : Flow_Type::SEQ;

        flow_.push(type);
        add_token(from_flow_type(type, true));
    }

    void Scanner::scan_flow_end()
    {
        Flow_Type type = next_char() == '}' ? Flow_Type::MAP : Flow_Type::SEQ;

        if (type != flow_.top())
            throw Parse_Exception(error_msgs::INVALID_FLOW_END, mark_);

        flow_.pop();
        add_token(from_flow_type(type, false));
        end_scalar();
    }

    void Scanner::scan_flow_entry()
    {
        next_char();
        add_token(Token_Type::FLOW_ENTRY);
    }

    void Scanner::scan_special_scalar()
    {
        assert(next() == '|' || next() == '>');

        // 换行替换字符
        if (next_char() == '|') {
            replace_ = '\n';
        }

        // 字符串末尾是否添加换行
        if (next() == '-') {
            append_ = false;
            next_char();
        } else if (is_delimiter(next()) && next() != -1) {
            append_ = true;
        } else {
            // 报错：
            throw Parse_Exception(error_msgs::NO_NEWLINE, mark_);
        }

        in_special_ = true;
        skip_to_next_token();
        scan_normal_scalar();
    }

    ///< @todo 处理带引号字符串为key的情况
    void Scanner::scan_quote_scalar()
    {
        assert(next() == '\'' || next() == '\"');
        char end_char = next_char();

        // 循环读取字符，直到 ' 或 "
        while (next() != end_char) {
            if (next() == -1) {
                throw Parse_Exception(error_msgs::EOF_IN_SCALAR, mark_);
            } else if (next() == '\\' && end_char == '\"') {
                // 转义字符处理
                value_ += escape();
            } else if (next() == '\n') {
                // 换行处理
                value_ += ' ';
                next_char();

                // 连续多个换行时不替换为空格
                while (next() == '\n') {
                    value_ += '\n';
                    next_char();
                }

                // 跳过每一行前面的空格
                while (next() == ' ') {
                    next_char();
                }
            } else {
                value_ += next_char();
            }
        }

        // 消耗右引号
        next_char();

        // 检查是否为 KEY
        bool can_be_key = false;
        while (!input_end_ && next() != '\n') {
            if (!is_delimiter(next()) && next() != ':')
                break;

            // 判断当前字符串属于 key 还是 value，如果是 key 则跳出
            if (match(":", true) ||
                !in_block() && match(":", std::string("]},"))) {
                can_be_key = true;
                break;
            }

            next_char();
        }

        if (can_be_key) {
            if (in_block()) {
                push_indent(Indent_Type::MAP);
                pop_indent();
                start_scalar();
            }
            add_token(Token_Type::KEY);
            add_token(value_);
        } else {
            add_token(value_);
            end_scalar();
        }
    }

    void Scanner::scan_normal_scalar()
    {
        bool can_be_key = false;
        bool hit_comment = false;
        bool hit_stop_char = false;

        std::string end_chars;
        if (!in_block()) {
            end_chars = ",";
            if (flow_.top() == Flow_Type::MAP)
                end_chars += '}';
            else if (flow_.top() == Flow_Type::SEQ)
                end_chars += ']';
        }

        while (!input_end_) {
            if (in_block() && get_cur_indent() < min_indent_)
                break;

            // 扫描字符串直到换行
            while (!input_end_ && next() != '\n') {
                // 遇到注释停止
                if (!in_special() && next() == '#') {
                    hit_comment = true;
                    break;
                }

                // 流节点中，遇到 FLOW_ENTRY 和 FLOW_END 停止扫描当前标量
                if (!in_block() && match_any_of(end_chars)) {
                    hit_stop_char = true;
                    break;
                }

                // 判断当前字符串属于 key 还是 value，如果是 key 则跳出
                if (!in_special() && next() == ':') {
                    if (match(":", true) ||
                        !in_block() && match(":", std::string("]},"))) {
                        can_be_key = true;
                        break;
                    }
                }

                // 接收字符
                value_ += next_char();
            }

            if (can_be_key || hit_comment || hit_stop_char)
                break;

            // 消耗换行符
            if (next() == '\n') {
                value_ += replace_;
                next_char();

                // 连续多个换行符不替换为空格
                while (next() == '\n') {
                    value_ += '\n';
                    next_char();
                }
            }

            // 检查 DOC_END
            if (match("...", true))
                break;

            // 消耗下一行的空格
            skip_blank();
        }

        // 跳过注释
        if (hit_comment || next() == '#') {
            skip_comment();
            skip_blank();
        }

        // 删除结尾空白字符
        value_ = value_.substr(0, value_.find_last_not_of(" \t\n\xFF") + 1);
        if (!value_.empty() && append_ && !can_be_key) {
            value_ += '\n';
        }

        // 跳过空字符串
        if (value_.empty() && !in_special()) {
            reset_scalar_flags();
            end_scalar();
            return;
        }

        // 特殊标量不可以作为 key
        if (can_be_key) {
            if (in_block()) {
                push_indent(Indent_Type::MAP);
                pop_indent();
                start_scalar();
            }
            add_token(Token_Type::KEY);
            add_token(value_);
        } else {
            // 特殊字符串不作为 null
            if (value_ != "~" && value_ != "null" || in_special()) {
                add_token(value_);
            }
            reset_scalar_flags();
            end_scalar();
        }
    }

} // namespace cyaml
