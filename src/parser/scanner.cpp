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
    Scanner::Scanner(std::istream &in): input_(in)
    {
        scan();
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
        assert(input_);

        char ret = input_.get();

        switch (ret) {
        // 换行
        case '\n':
            tab_cnt_ = 0;
            ignore_tab_ = true;
            after_anchor_ = false;
            break;

        // yaml 不允许制表符缩进，需要记录
        case '\t':
            if (ignore_tab_)
                tab_cnt_++;
            break;
        }

        return ret;
    }

    void Scanner::skip_to_next_token()
    {
        while (input_ &&
               (is_delimiter(input_.peek()) || input_.peek() == '#')) {
            skip_blank();
            skip_comment();
        }
        update_indent();
    }

    void Scanner::scan()
    {
        skip_to_next_token();

        // STREAM_END
        if (!input_) {
            scan_end_ = true;
            return stream_end();
        }

        // DOC
        if (input_.column() == 1 && match("---", Match_End::ANY))
            return scan_doc_start();

        if (input_.column() == 1 && match("...", Match_End::BLANK))
            return scan_doc_end();

        // ANCHOR, ALIAS
        if (input_.peek() == '&')
            return scan_anchor();

        if (input_.peek() == '*')
            return scan_alias();

        // BLOCK_ENTRY
        if (match("-", Match_End::BLANK))
            return scan_block_entry();

        // FLOW START AND END
        if (input_.peek() == '{' || input_.peek() == '[')
            return scan_flow_start();

        if (input_.peek() == '}' || input_.peek() == ']')
            return scan_flow_end();

        // FLOW_ENTRY
        if (input_.peek() == ',')
            return scan_flow_entry();

        // KEY
        if (in_block() && match("?", Match_End::BLANK))
            return scan_key();

        // VALUE
        if (match_value())
            return scan_value();

        if (in_block() && (input_.peek() == '|' || input_.peek() == '>'))
            return scan_special_scalar();

        if (input_.peek() == '\'' || input_.peek() == '\"')
            return scan_quote_scalar();

        if (!is_delimiter(input_.peek()) &&
            !match_any_of(",[]{}#&*!|>\'\"%@`"))
            return scan_normal_scalar();

        throw Parse_Exception(error_msgs::UNKNOWN_TOKEN, token_mark());
    }

    void Scanner::update_indent()
    {
        ignore_tab_ = false;
        cur_indent_ = get_cur_indent();
        token_mark_ = input_.mark();
    }

    char Scanner::escape()
    {
        char slash = next_char();
        assert(slash == '\\');

        if (auto iter = escape_map.find(input_.peek());
            iter != escape_map.end()) {
            next_char();
            return iter->second;
        }

        throw Parse_Exception(error_msgs::UNKNOWN_ESCAPE, input_.mark());
    }

    void Scanner::push_indent(Indent_Type type)
    {
        uint32_t len = after_anchor_ ? anchor_indent_ : cur_indent_;
        if (indent_.empty() || len > indent_.top().len) {
            add_token(from_indent_type(type, true));
            indent_.push({type, len});
        }
    }

    void Scanner::pop_indent()
    {
        if (indent_.empty())
            return;

        uint32_t len = after_anchor_ ? anchor_indent_ : cur_indent_;
        while (!indent_.empty() && len < indent_.top().len) {
            add_token(from_indent_type(indent_.top().type, false));
            indent_.pop();
        }

        if (indent_.empty() || len != indent_.top().len)
            throw Parse_Exception(error_msgs::INVALID_INDENT, token_mark());
    }

    bool Scanner::match(std::string pattern, Match_End end)
    {
        bool need_blank = (end == Match_End::BLANK);
        int size = pattern.size() + (need_blank ? 1 : 0);
        if (!input_.read_to(size))
            return false;

        for (int i = 0; i < pattern.size(); i++) {
            if (input_.at(i) != pattern[i])
                return false;
        }

        if (need_blank && !is_delimiter(input_.at(size - 1)))
            return false;

        return true;
    }

    bool Scanner::match(std::string pattern1, std::string pattern2)
    {
        int size = pattern1.size() + 1;
        if (!input_.read_to(size))
            return false;

        for (int i = 0; i < pattern1.size(); i++) {
            if (input_.at(i) != pattern1[i])
                return false;
        }

        return pattern2.find(input_.at(size - 1)) != -1;
    }

} // namespace cyaml
