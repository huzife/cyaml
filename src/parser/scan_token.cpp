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
        can_be_json = false;

        // 匹配剩下的缩进
        pop_all_indent();
    }

    void Scanner::scan_doc_start()
    {
        can_be_json = false;

        for (auto i = 0; i < 3; i++) {
            next_char();
        }

        add_token(Token_Type::DOC_START);
    }

    void Scanner::scan_doc_end()
    {
        can_be_json = false;

        pop_all_indent();

        for (int i = 0; i < 3; i++) {
            next_char();
        }

        add_token(Token_Type::DOC_END);
    }

    void Scanner::scan_anchor()
    {
        std::string value;
        can_be_json = false;

        // '&'
        next_char();

        while (input_) {
            if (is_delimiter(input_.peek()) || match_any_of("[]{},"))
                break;
            value += next_char();
        }

        if (value.empty()) {
            throw Parse_Exception(error_msgs::EMPTY_ANCHOR, input_.mark());
        }

        if (!is_delimiter(input_.peek()) && !match_any_of("?:,]}%@`")) {
            throw Parse_Exception(error_msgs::END_OF_ANCHOR, input_.mark());
        }

        // 记录当前缩进
        anchor_indent_ = cur_indent_;
        after_anchor_ = true;

        add_token(Token_Type::ANCHOR, value);
    }

    void Scanner::scan_alias()
    {
        std::string value;
        can_be_json = false;

        // '*'
        next_char();

        while (input_) {
            if (is_delimiter(input_.peek()) || match_any_of("[]{},"))
                break;
            value += next_char();
        }

        if (value.empty()) {
            throw Parse_Exception(error_msgs::EMPTY_ALIAS, input_.mark());
        }

        if (!is_delimiter(input_.peek()) && !match_any_of("?:,]}%@`")) {
            throw Parse_Exception(error_msgs::END_OF_ANCHOR, input_.mark());
        }

        skip_blank();
        if (match(":", Match_End::BLANK)) {
            push_indent(Indent_Type::MAP);
            pop_indent();
            start_scalar();
            add_token(Token_Type::KEY);
        }

        add_token(Token_Type::ALIAS, value);
    }

    void Scanner::scan_block_entry()
    {
        can_be_json = false;

        next_char();
        push_indent(Indent_Type::SEQ);
        pop_indent();
        start_scalar();
        add_token(Token_Type::BLOCK_ENTRY);
    }

    void Scanner::scan_key()
    {
        can_be_json = false;

        next_char();
        push_indent(Indent_Type::MAP);
        pop_indent();
        start_scalar();
        add_token(Token_Type::KEY);
    }

    void Scanner::scan_value()
    {
        can_be_json = false;

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
        can_be_json = false;

        Flow_Type type = next_char() == '{' ? Flow_Type::MAP : Flow_Type::SEQ;

        flow_.push(type);
        add_token(from_flow_type(type, true));
    }

    void Scanner::scan_flow_end()
    {
        can_be_json = false;

        Flow_Type type = next_char() == '}' ? Flow_Type::MAP : Flow_Type::SEQ;

        if (type != flow_.top())
            throw Parse_Exception(error_msgs::INVALID_FLOW_END, token_mark());

        flow_.pop();
        add_token(from_flow_type(type, false));
        end_scalar();
    }

    void Scanner::scan_flow_entry()
    {
        can_be_json = false;

        next_char();
        add_token(Token_Type::FLOW_ENTRY);
    }

    void Scanner::scan_special_scalar()
    {
        can_be_json = false;

        assert(input_.peek() == '|' || input_.peek() == '>');

        // 换行替换字符
        if (next_char() == '|') {
            replace_ = '\n';
        }

        // 字符串末尾是否添加换行
        char next = input_.peek();
        if (next == '-') {
            append_ = false;
            next_char();
        } else if (is_delimiter(next) && next != Stream::eof) {
            append_ = true;
        } else {
            // 报错：
            throw Parse_Exception(error_msgs::NO_NEWLINE, input_.mark());
        }

        in_special_ = true;
        skip_to_next_token();
        scan_normal_scalar();
    }

    ///< @todo 处理带引号字符串为key的情况
    void Scanner::scan_quote_scalar()
    {
        std::string value;
        can_be_json = true;

        assert(input_.peek() == '\'' || input_.peek() == '\"');
        char end_char = next_char();

        // 循环读取字符，直到 ' 或 "
        while (input_.peek() != end_char) {
            if (input_.peek() == Stream::eof) {
                throw Parse_Exception(
                        error_msgs::EOF_IN_SCALAR, input_.mark());
            } else if (input_.peek() == '\\' && end_char == '\"') {
                // 转义字符处理
                value += escape();
            } else if (input_.peek() == '\n') {
                // 换行处理
                value += ' ';
                next_char();

                // 连续多个换行时不替换为空格
                while (input_.peek() == '\n') {
                    value += '\n';
                    next_char();
                }

                // 跳过每一行前面的空格
                while (input_.peek() == ' ') {
                    next_char();
                }
            } else {
                value += next_char();
            }
        }

        // 消耗右引号
        next_char();

        // 检查是否为 KEY
        bool can_be_key = false;
        while (input_ && input_.peek() != '\n') {
            if (!is_delimiter(input_.peek()) && input_.peek() != ':')
                break;

            // 判断当前字符串属于 key 还是 value，如果是 key 则跳出
            if (match_value()) {
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
            add_token(Token_Type::SCALAR, value);
        } else {
            add_token(Token_Type::SCALAR, value);
            end_scalar();
        }
    }

    void Scanner::scan_normal_scalar()
    {
        std::string value;
        can_be_json = false;

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

        while (input_) {
            if (in_block() && get_cur_indent() < min_indent_)
                break;

            // 扫描字符串直到换行
            while (input_ && input_.peek() != '\n') {
                // 遇到注释停止
                if (!in_special() && match(" #", Match_End::ANY)) {
                    hit_comment = true;
                    break;
                }

                // 流节点中，遇到 FLOW_ENTRY 和 FLOW_END 停止扫描当前标量
                if (!in_block() && match_any_of(end_chars)) {
                    hit_stop_char = true;
                    break;
                }

                // 判断当前字符串属于 key 还是 value，如果是 key 则跳出
                if (!in_special() && input_.peek() == ':') {
                    if (match_value()) {
                        can_be_key = true;
                        break;
                    }
                }

                // 接收字符
                value += next_char();
            }

            if (can_be_key || hit_comment || hit_stop_char)
                break;

            // 消耗换行符
            if (input_.peek() == '\n') {
                value += replace_;
                next_char();

                // 连续多个换行符不替换为空格
                while (input_.peek() == '\n') {
                    value += '\n';
                    next_char();
                }
            }

            // 检查 DOCUMENT INDICATOR
            if (match("...", Match_End::BLANK) ||
                match("---", Match_End::BLANK))
                break;

            // 消耗下一行的空格
            skip_blank();
        }

        // 跳过注释
        if (hit_comment || input_.peek() == '#') {
            skip_comment();
            skip_blank();
        }

        // 删除结尾空白字符
        value = value.substr(0, value.find_last_not_of(" \t\n\xFF") + 1);
        if (!value.empty() && append_ && !can_be_key) {
            value += '\n';
        }

        // 跳过空字符串
        if (value.empty() && !in_special()) {
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
            add_token(Token_Type::SCALAR, value);
        } else {
            // 特殊字符串不作为 null
            if (value != "~" && value != "null" || in_special()) {
                add_token(Token_Type::SCALAR, value);
            }
            reset_scalar_flags();
            end_scalar();
        }
    }

} // namespace cyaml
