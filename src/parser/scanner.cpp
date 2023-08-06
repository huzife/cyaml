/**
 * @file        scanner.cpp
 * @brief       YAML 解析器词法分析部分 Scanner 类源文件
 * @details     主要包含 YAML 的 Scanner 类实现
 * @date        2023-7-20
 */

#include "parser/scanner.h"
#include "type/tables.h"
#include "error/exceptions.h"
#include <assert.h>
#include <algorithm>
#include <iostream>

namespace cyaml
{
    Scanner::Scanner(std::istream &in): input_stream_(in)
    {
        chars_.push_back(input_stream_.get());
        add_token(Token_Type::STREAM_START);
    }

    Token Scanner::next_token()
    {
        while (!scan_end_ && token_.size() < 2) {
            scan();
        }

        if (token_.empty())
            return Token();

        Token ret = token_.front();
        token_.pop();
        return ret;
    }

    Token Scanner::lookahead() const
    {
        if (token_.empty())
            return Token();
        else
            return token_.front();
    }

    char Scanner::next_char()
    {
        assert(!input_end_);

        if (!input_stream_.eof())
            chars_.push_back(input_stream_.get());
        char ret = next();

        switch (ret) {
        // 换行
        case '\n':
            mark_.line++;
            mark_.column = 1;
            tab_cnt_ = 0;
            ignore_tab_ = true;
            break;

        // yaml 不允许制表符缩进，需要记录
        case '\t':
            mark_.column++;
            if (ignore_tab_)
                tab_cnt_++;
            break;

        // 文件结束符
        case -1:
            input_end_ = true;
            break;

        default:
            mark_.column++;
            break;
        }

        if (!input_end_)
            chars_.pop_front();

        return ret;
    }

    void Scanner::skip_to_next_token()
    {
        value_.clear();
        while (!input_end_ && (is_delimiter(next()) || next() == '#')) {
            skip_blank();
            skip_comment();
        }
        update_indent();
    }

    void Scanner::scan()
    {
        skip_to_next_token();

        // STREAM_END
        if (next() == -1) {
            scan_end_ = true;
            return stream_end();
        }

        // DOC
        if (mark_.column == 1 && match("---"))
            return scan_doc_start();

        if (mark_.column == 1 && match("...", true))
            return scan_doc_end();

        // BLOCK_ENTRY
        if (match("-", true))
            return scan_block_entry();

        // FLOW START AND END
        if (next() == '{' || next() == '[')
            return scan_flow_start();

        if (next() == '}' || next() == ']')
            return scan_flow_end();

        // FLOW_ENTRY
        if (next() == ',')
            return scan_flow_entry();

        // KEY
        if (in_block() && match("?", true))
            return scan_key();

        // VALUE
        if (match(":", true) || !in_block() && match(":", std::string("]},")))
            return scan_value();

        if (in_block() && (next() == '|' || next() == '>'))
            return scan_special_scalar();

        if (next() == '\'' || next() == '\"')
            return scan_quote_scalar();

        if (!is_delimiter(next()) && !match_any_of(",[]{}#&*!|>\'\"%@`"))
            return scan_normal_scalar();

        throw Parse_Exception(error_msgs::UNKNOWN_TOKEN, token_mark());
    }

    void Scanner::update_indent()
    {
        ignore_tab_ = false;
        cur_indent_ = get_cur_indent();
        token_mark_ = mark();
    }

    char Scanner::escape()
    {
        char slash = next_char();
        assert(slash == '\\');

        if (auto iter = escape_map.find(next()); iter != escape_map.end()) {
            next_char();
            return iter->second;
        }

        throw Parse_Exception(error_msgs::UNKNOWN_ESCAPE, mark());
    }

    void Scanner::push_indent(Indent_Type type)
    {
        if (indent_.empty() || cur_indent_ > indent_.top().len) {
            add_token(from_indent_type(type, true));
            indent_.push({type, cur_indent_});
        }
    }

    void Scanner::pop_indent()
    {
        if (indent_.empty())
            return;

        while (!indent_.empty() && cur_indent_ < indent_.top().len) {
            add_token(from_indent_type(indent_.top().type, false));
            indent_.pop();
        }

        if (indent_.empty() || cur_indent_ != indent_.top().len)
            throw Parse_Exception(error_msgs::INVALID_INDENT, token_mark());
    }

    bool Scanner::match(std::string pattern, bool end_with_delimiter)
    {
        bool ret = true;
        std::stack<char> temp_chars;
        for (char ch : pattern) {
            if (!input_stream_.eof() && !chars_.empty())
                chars_.push_back(input_stream_.get());

            if (ch != next()) {
                ret = false;
                break;
            }

            temp_chars.push(next());
            chars_.pop_front();
        }

        if (end_with_delimiter && !is_delimiter(next()))
            ret = false;

        while (!temp_chars.empty()) {
            char ch = temp_chars.top();
            chars_.push_front(ch);
            temp_chars.pop();
        }

        return ret;
    }

    bool Scanner::match(std::string pattern1, std::string pattern2)
    {
        bool ret = true;
        std::stack<char> temp_chars;
        for (char ch : pattern1) {
            if (!input_stream_.eof() && !chars_.empty())
                chars_.push_back(input_stream_.get());

            if (ch != next()) {
                ret = false;
                break;
            }

            temp_chars.push(next());
            chars_.pop_front();
        }

        ret &= match_any_of(pattern2);

        while (!temp_chars.empty()) {
            char ch = temp_chars.top();
            chars_.push_front(ch);
            temp_chars.pop();
        }

        return ret;
    }

} // namespace cyaml
