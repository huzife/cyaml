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
        last_token_type_ = Token_Type::STREAM_START;
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
        if (match(":", "[]{},#|>\'\""))
            return scan_value();

        if (in_block() && (next() == '|' || next() == '>'))
            return scan_special_scalar();

        if (next() == '\'' || next() == '\"')
            return scan_quote_scalar();

        if (!match_any_of("[]{},#|>\'\""))
            return scan_normal_scalar();

        throw Parse_Exception(error_msgs::UNKNOWN_TOKEN, mark_);
    }

    void Scanner::fill_null(Indent_Type type)
    {
        if (need_scalar_ && !indent_.empty() &&
            cur_indent_ <= indent_.top().len && type == indent_.top().type) {
            need_scalar_ = false;
            add_token(Token::null());
        }
    }

    void Scanner::fill_null(Flow_Type type)
    {
        if (last_token_type_ != Token_Type::FLOW_ENTRY &&
            last_token_type_ != Token_Type::FLOW_MAP_START &&
            last_token_type_ != Token_Type::FLOW_SEQ_START)
            return;

        if (type == Flow_Type::MAP) {
            add_token(Token_Type::KEY);
            add_token(Token("null"));
            add_token(Token_Type::VALUE);
        }
        add_token(Token::null());
    }

    void Scanner::update_indent()
    {
        ignore_tab_ = false;
        cur_indent_ = get_cur_indent();
    }

    char Scanner::escape()
    {
        char slash = next_char();
        assert(slash == '\\');

        if (auto iter = escape_map.find(next()); iter != escape_map.end()) {
            next_char();
            return iter->second;
        }

        throw Parse_Exception(error_msgs::UNKNOWN_ESCAPE, mark_);
    }

    void Scanner::push_indent(Indent_Type type)
    {
        Indent indent{type, cur_indent_};
        if (indent_.empty() || cur_indent_ > indent_.top().len) {
            add_token(type, true);
            indent_.push(indent);
        }
    }

    void Scanner::pop_indent()
    {
        if (indent_.empty())
            return;

        uint32_t len = get_cur_indent();
        while (!indent_.empty() && len != indent_.top().len) {
            add_token(indent_.top().type, false);
            indent_.pop();
        }

        if (indent_.empty())
            throw Parse_Exception(error_msgs::INVALID_INDENT, mark_);
    }

    bool Scanner::match(std::string pattern, bool end_with_delimiter)
    {
        bool ret = true;
        std::stack<char> chars;
        for (char ch : pattern) {
            if (!input_stream_.eof())
                chars_.push_back(input_stream_.get());

            if (ch != next()) {
                ret = false;
                break;
            }

            chars.push(next());
            chars_.pop_front();
        }

        if (end_with_delimiter && !is_delimiter(next()))
            ret = false;

        while (!chars.empty()) {
            char ch = chars.top();
            chars_.push_front(ch);
            chars.pop();
        }

        return ret;
    }

    bool Scanner::match(std::string pattern1, std::string pattern2)
    {
        bool ret = true;
        std::stack<char> chars;
        for (char ch : pattern1) {
            if (!input_stream_.eof())
                chars_.push_back(input_stream_.get());

            if (ch != next()) {
                ret = false;
                break;
            }

            chars.push(next());
            chars_.pop_front();
        }

        ret &= match_any_of(pattern2);

        while (!chars.empty()) {
            char ch = chars.top();
            chars_.push_front(ch);
            chars.pop();
        }

        return ret;
    }

} // namespace cyaml
