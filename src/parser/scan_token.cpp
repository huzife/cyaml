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
        // 检查最后一个标量是否 null
        if (need_scalar_) {
            add_token(Token_Type::VALUE);
            add_token(Token::null());
        }

        // 匹配剩下的缩进
        pop_all_indent();

        // 添加 STREAM_END
        add_token(Token_Type::STREAM_END);
    }

    void Scanner::scan_doc_start()
    {
        for (auto i = 0; i < 3; i++) {
            if (next() != '-')
                throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);
            next_char();
        }

        add_token(Token_Type::DOC_START);
    }

    void Scanner::scan_doc_end()
    {
        pop_all_indent();

        for (int i = 0; i < 3; i++) {
            if (next() != '.')
                throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);
            next_char();
        }

        if (!is_delimiter(next()))
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);

        add_token(Token_Type::DOC_END);
    }

    void Scanner::scan_block_seq_entry()
    {
        bool success = false;
        if (next() == '-') {
            next_char();
            if (is_delimiter(next()))
                success = true;
        }

        if (!success) {
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);
        }

        fill_null(Indent_Type::SEQ);
        start_scalar();
        push_indent(Indent_Type::SEQ);
        add_token(Token_Type::BLOCK_ENTRY);
    }

    void Scanner::scan_flow_start()
    {
        if (next() != '{' && next() != '[')
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);

        Flow_Type type = next_char() == '{' ? Flow_Type::MAP : Flow_Type::SEQ;

        flow_.push(type);
        add_token(from_flow_type(type, true));
    }

    void Scanner::scan_flow_end()
    {
        if (next() != '}' && next() != ']')
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);

        Flow_Type type = next_char() == '}' ? Flow_Type::MAP : Flow_Type::SEQ;

        if (type != flow_.top())
            throw Parse_Exception(error_msgs::INVALID_FLOW_END, mark_);

        flow_.pop();
        add_token(from_flow_type(type, false));

        end_scalar();
    }

    void Scanner::scan_flow_entry()
    {
        if (next() != ',')
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);

        next_char();

        // 连续两个 FLOW_ENTRY 中间补充 null
        fill_null(flow_.top());
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
        bool is_key = false;
        while (!input_end_ && next() != '\n') {
            if (!is_delimiter(next()) && next() != ':')
                break;

            if (next_char() == ':' &&
                (is_delimiter(next()) ||
                 !in_block() && match_any_of("[]{},#|>\'\""))) {
                is_key = true;
                break;
            }
        }

        skip_blank();
        if (in_block()) {
            if (is_key) {
                // 识别为 KEY
                fill_null(Indent_Type::MAP);
                start_scalar();
                push_indent(Indent_Type::MAP);
                add_token(Token_Type::KEY);
                add_token(value_);
            } else {
                // 识别为标量
                add_token(Token_Type::VALUE);
                add_token(value_);
                end_scalar();
                pop_indent();
            }
        } else {
            if (is_key) {
                add_token(Token_Type::KEY);
                add_token(value_);
            } else {
                add_token(Token_Type::VALUE);
                add_token(value_);
            }
        }
    }

    void Scanner::scan_normal_scalar()
    {
        bool is_key = false;
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
                if (!in_special_ && next() == '#') {
                    hit_comment = true;
                    break;
                }

                // 流节点中，遇到 FLOW_ENTRY 和 FLOW_END 停止扫描当前标量
                if (!in_block() && match_any_of(end_chars)) {
                    hit_stop_char = true;
                    break;
                }

                char ch = next_char();

                // 判断当前字符串属于 key 还是 value，如果是 key 则跳出
                if (ch == ':' &&
                    (is_delimiter(next()) ||
                     !in_block() && match_any_of("[]{},#|>\'\""))) {
                    is_key = true;
                    break;
                }

                // 接收字符
                value_ += ch;
            }

            if (is_key || hit_comment || hit_stop_char)
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
        if (!value_.empty() && append_ && !is_key) {
            value_ += '\n';
        }

        if (in_block()) {
            // 跳过空字符串
            if (!in_special_ && value_.empty()) {
                min_indent_ = 0;
                reset_scalar_flags();
                return;
            }

            if (is_key) {
                // 识别为 KEY
                fill_null(Indent_Type::MAP);
                start_scalar();
                push_indent(Indent_Type::MAP);
                add_token(Token_Type::KEY);
                add_token(value_);
            } else {
                add_token(Token_Type::VALUE);
                // 只有在 block 内可以使用特殊字符串，此时标量不识别为 null
                if ((value_ == "~" || value_ == "null") && !in_special_) {
                    add_token(Token::null());
                } else {
                    add_token(value_);
                }
                end_scalar();
                pop_indent();
            }

            reset_scalar_flags();
        } else {
            if (is_key) {
                add_token(Token_Type::KEY);
                add_token(value_);
            } else {
                add_token(Token_Type::VALUE);
                if (value_ == "~" || value_ == "null") {
                    add_token(Token::null());
                } else {
                    add_token(value_);
                }
            }
        }
    }

} // namespace cyaml
