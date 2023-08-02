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
            token_.push(Token(Token_Type::SCALAR, ""));
        }

        // 匹配剩下的缩进
        pop_all_indent();

        // 添加 STREAM_END
        token_.push(Token(Token_Type::STREAM_END));
    }

    void Scanner::scan_doc_start()
    {
        for (auto i = 0; i < 3; i++) {
            if (next() != '-')
                throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);
            value_ += next_char();
        }

        token_.push(Token(Token_Type::DOC_START, value_));
    }

    void Scanner::scan_doc_end()
    {
        pop_all_indent();

        for (int i = 0; i < 3; i++) {
            if (next() != '.')
                throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);
            value_ += next_char();
        }

        if (!is_delimiter(next()))
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);

        token_.push(Token(Token_Type::DOC_END, value_));
    }

    void Scanner::scan_block_seq_entry()
    {
        bool success = false;
        if (next() == '-') {
            value_ += next_char();
            if (is_delimiter(next()))
                success = true;
        }

        if (!success) {
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);
        }

        fill_null(Indent_Type::SEQ);
        start_scalar();
        push_indent(Indent_Type::SEQ);
        token_.push(Token(Token_Type::BLOCK_SEQ_ENTRY, value_));
    }

    void Scanner::scan_flow_start()
    {
        if (next() != '{' && next() != '[')
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);

        Token_Type type = next() == '{' ? Token_Type::FLOW_MAP_START
                                        : Token_Type::FLOW_SEQ_START;
        value_ += next_char();
        token_.push(Token(type, value_));
    }

    void Scanner::scan_flow_end()
    {
        if (next() != '}' && next() != ']')
            throw Parse_Exception(error_msgs::SCAN_TOKEN_ERROR, mark_);

        Token_Type type = next() == '}' ? Token_Type::FLOW_MAP_END
                                        : Token_Type::FLOW_SEQ_END;
        value_ += next_char();
        token_.push(Token(type, value_));
    }

    void Scanner::scan_scalar()
    {
        if (next() == '|' || next() == '>') {
            scan_special_scalar();
        } else if (next() == '\'' || next() == '\"') {
            scan_quote_scalar();
        } else {
            scan_normal_scalar();
        }
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

            if (next_char() == ':' && is_delimiter(next())) {
                is_key = true;
                break;
            }
        }

        skip_blank();

        // 识别为 KEY
        if (is_key) {
            fill_null(Indent_Type::MAP);
            start_scalar();
            push_indent(Indent_Type::MAP);
            token_.push(Token(Token_Type::KEY, value_));
            return;
        }

        token_.push(Token(value_));
        end_scalar();
        pop_indent();
    }

    void Scanner::scan_normal_scalar()
    {
        bool is_key = false;
        bool hit_comment = false;

        while (!input_end_ && mark_.column - tab_cnt_ - 1 >= min_indent_) {
            // 扫描字符串直到换行
            while (!input_end_ && next() != '\n') {
                // 遇到注释停止
                if (!in_special_ && next() == '#') {
                    hit_comment = true;
                    break;
                }

                // 判断当前字符串属于 key 还是 value，如果是 key 则跳出
                char ch = next_char();
                if (ch == ':' && is_delimiter(next())) {
                    is_key = true;
                    break;
                }

                // 接收字符
                value_ += ch;
            }

            if (is_key || hit_comment)
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

        // 跳过空字符串
        if (value_.empty()) {
            min_indent_ = 0;
            return;
        }

        reset_scalar_flags();

        // 识别为 KEY
        if (is_key) {
            fill_null(Indent_Type::MAP);
            start_scalar();
            push_indent(Indent_Type::MAP);
            token_.push(Token(Token_Type::KEY, value_));
            return;
        }

        // 识别为标量
        token_.push(Token(value_));
        end_scalar();
        pop_indent();
    }

} // namespace cyaml
