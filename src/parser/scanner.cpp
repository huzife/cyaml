/**
 * @file        scanner.cpp
 * @brief       YAML 解析器词法分析部分 Scanner 类源文件
 * @details     主要包含 YAML 的 Scanner 类实现
 * @date        2023-7-20
 */

#include "parser/scanner.h"
#include "error/exceptions.h"
#include <assert.h>
#include <iostream>

namespace cyaml
{
    Scanner::Scanner(std::istream &in): input_stream_(in)
    {
        char_.push_back(input_stream_.get());
        token_.push(Token(Token_Type::STREAM_START));
    }

    Token Scanner::next_token()
    {
        Token ret;
        if (token_.empty())
            ret = Token();
        else {
            ret = token_.front();
            token_.pop();
        }

        if (!scan_end_)
            scan();
        return ret;
    }

    Token Scanner::lookahead()
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
            char_.push_back(input_stream_.get());
        char ret = char_.front();

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
            char_.pop_front();

        return ret;
    }

    void Scanner::scan()
    {
        value_.clear();

        // 跳过\t和\n，暂时跳过空格
        while (!input_end_ && is_delimiter(char_.front())) {
            next_char();
        }

        update_indent();

        if (char_.front() == -1) {
            scan_end_ = true;

            // 检查最后一个标量是否 null
            if (!get_scalar_) {
                scan_scalar();
            }

            // 匹配剩下的缩进
            while (!indent_.empty()) {
                token_.push(Token(indent_.top(), false));
                indent_.pop();
            }

            // 添加 STREAM_END
            token_.push(Token(Token_Type::STREAM_END));
        } else if (is_operator(char_.front())) {
            scan_operator();
        } else {
            scan_scalar();
        }
    }

    void Scanner::update_indent()
    {
        ignore_tab_ = false;
        cur_indent_ = mark_.column - tab_cnt_ - 1;
    }

    void Scanner::scan_scalar()
    {
        if (char_.front() == '|' || char_.front() == '>') {
            scan_special_string();
        } else if (char_.front() == '\'' || char_.front() == '\"') {
            scan_quote_string();
        } else {
            scan_normal_string();
        }
    }

    void Scanner::scan_operator()
    {
        Token_Type token_type;
        bool is_operator_ = false;

        char ch = next_char();
        value_ += ch;

        switch (ch) {
        case '-':
            if (is_delimiter(char_.front())) {
                push_indent(Indent_Type::SEQ);
                token_type = Token_Type::DASH;
            } else {
                // 读取后两个字符，判断是否匹配'---'
                for (int i = 0; i < 2; i++) {
                    if (char_.front() != '-')
                        break;

                    value_ += next_char();
                }
                token_type = Token_Type::DOC_START;
                is_operator_ = true;
            }
            break;
        case ':':
            token_type = Token_Type::COLON;
            break;
        case '{':
            token_type = Token_Type::LBRACE;
            break;
        case '}':
            token_type = Token_Type::RBRACE;
            break;
        case '[':
            token_type = Token_Type::LBRACKET;
            break;
        case ']':
            token_type = Token_Type::RBRACKET;
            break;
        }

        if (is_operator_ || is_delimiter(char_.front()))
            token_.push(Token(token_type, value_));
        else
            scan_scalar();
    }

    void Scanner::scan_special_string()
    {
        assert(char_.front() == '|' || char_.front() == '>');

        // 换行替换字符
        if (next_char() == '|') {
            replace_ = '\n';
        }

        // 字符串末尾是否添加换行
        if (char_.front() == '-') {
            append_ = false;
            next_char();
        } else if (is_delimiter(char_.front()) && char_.front() != -1) {
            append_ = true;
        } else {
            // 报错：
            throw Parse_Exception(error_msgs::NO_NEWLINE, mark_);
        }

        scan();
    }

    void Scanner::scan_quote_string()
    {
        assert(char_.front() == '\'' || char_.front() == '\"');
        char end_char = next_char();
        String_Type string_type = end_char == '\''
                                          ? String_Type::SQUOTE_STRING
                                          : String_Type::DQUOTE_STRING;

        // 循环读取字符，直到 ' 或 "
        while (char_.front() != end_char) {
            if (char_.front() == -1) {
                throw Parse_Exception(error_msgs::EOF_IN_SCALAR, mark_);
            } else if (char_.front() == '\\' && end_char == '\"') {
                // 转义字符处理
                value_ += escape();
            } else if (char_.front() == '\n') {
                // 换行处理
                value_ += ' ';
                next_char();

                // 连续多个换行时不替换为空格
                while (char_.front() == '\n') {
                    value_ += '\n';
                    next_char();
                }
                while (char_.front() == ' ') {
                    next_char();
                }
            } else {
                value_ += next_char();
            }
        }

        // 消耗多出的引号
        next_char();
        while (!input_end_ && is_delimiter(char_.front())) {
            next_char();
        }

        get_scalar_ = true;
        token_.push(Token(string_type, value_));
        pop_indent();
    }

    void Scanner::scan_normal_string(char replace, bool append_newline)
    {
        bool is_key = false;

        while (!input_end_ && mark_.column - tab_cnt_ - 1 >= min_indent_) {
            while (!input_end_ && char_.front() != '\n') {
                char ch = next_char();

                // 判断当前字符串属于 key 还是 value，如果是 key 则跳出
                if (ch == ':' && is_delimiter(char_.front())) {
                    char_.push_front(ch);
                    mark_.column--;
                    is_key = true;
                    break;
                } else {
                    value_ += ch;
                }
            }

            if (is_key)
                break;

            // 消耗换行符
            if (char_.front() == '\n') {
                value_ += replace_;
                next_char();
                // 连续多个换行符不替换为空格
                while (char_.front() == '\n') {
                    value_ += '\n';
                    next_char();
                }
            }

            // 消耗下一行的空格
            while (!input_end_ && char_.front() == ' ') {
                next_char();
            }
        }

        value_ = value_.substr(0, value_.find_last_not_of(" \t\n\xFF") + 1);
        if (!value_.empty() && append_ && !is_key)
            value_ += '\n';

        replace_ = ' ';
        append_ = false;

        if (is_key) {
            push_indent(Indent_Type::MAP);
            token_.push(Token(Token_Type::KEY, value_));
        } else {
            get_scalar_ = true;
            token_.push(Token(String_Type::NORMAL_STRING, value_));
            pop_indent();
        }
    }

    char Scanner::escape()
    {
        char slash = next_char();
        assert(slash == '\\');

        switch (next_char()) {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case '0':
            return '\0';
        default:
            throw Parse_Exception(error_msgs::UNKNOWN_ESCAPE, mark_);
        }
    }

    void Scanner::push_indent(Indent_Type type)
    {
        min_indent_ = cur_indent_ + 1;
        get_scalar_ = false;

        Indent indent{type, cur_indent_};
        if (indent_.empty() || cur_indent_ > indent_.top().len) {
            token_.push(Token(indent, true));
            indent_.push(indent);
        }
    }

    void Scanner::pop_indent()
    {
        min_indent_ = 0;

        if (mark_.column == 1 && indent_.empty())
            return;

        uint32_t len = mark_.column - tab_cnt_ - 1;
        while (!indent_.empty() && len != indent_.top().len) {
            token_.push(Token(indent_.top(), false));
            indent_.pop();
        }

        if (indent_.empty())
            throw Parse_Exception(error_msgs::INVALID_INDENT, mark_);
    }

} // namespace cyaml
