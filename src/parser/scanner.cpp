/**
 * @file        scanner.cpp
 * @brief       YAML 解析器词法分析部分 Scanner 类源文件
 * @details     主要包含 YAML 的 Scanner 类实现
 * @date        2023-7-20
 */

#include "parser/scanner.h"
#include <assert.h>
#include <iostream>

namespace cyaml
{
    Scanner::Scanner(std::istream &in): input_stream_(in)
    {
        next_char_ = input_stream_.get();
        scan();
    }

    Token Scanner::next_token()
    {
        Token ret = next_token_;
        scan();
        return ret;
    }

    Token Scanner::lookahead()
    {
        assert(next_token_.token_type() != Token_Type::NONE);
        return next_token_;
    }

    char Scanner::next_char()
    {
        if (next2_valid_) {
            next2_valid_ = false;
            char ret = next_char_;
            next_char_ = next2_char_;
            return ret;
        }

        assert(!input_end_);

        char ret = next_char_;
        next_char_ = input_stream_.get();

        switch (ret) {
        // 换行
        case '\n':
            line_++;
            col_ = 1;
            tab_cnt_ = 0;
            ignore_tab_ = true;
            break;

        // yaml 不允许制表符缩进，需要记录
        case '\t':
            col_++;
            if (ignore_tab_)
                tab_cnt_++;
            break;

        // 文件结束符
        case -1:
            input_end_ = true;
            break;

        default:
            col_++;
            break;
        }

        return ret;
    }

    void Scanner::scan()
    {
        value_.clear();

        // 跳过\t和\n，暂时跳过空格
        while (!input_end_ && is_delimiter(next_char_)) {
            next_char();
        }

        update_indent();

        if (next_char_ == -1) {
            scan_end_ = true;
        } else if (normal_) {
            normal_ = false;
            scan_normal_string();
        } else if (is_number(next_char_) || is_string_begin(next_char_)) {
            scan_scalar();
        } else if (is_operator(next_char_)) {
            scan_operator();
        } else {
            // TODO: 报错或其他
        }

        // 解析出一个 key 或 '-'，更新对标量的缩进限制
        Token_Type type = next_token_.token_type();
        if (type == Token_Type::SERIES_ENTRY || type == Token_Type::KEY) {
            min_indent_ = next_token_.indent() + 1;
        } else if (type == Token_Type::SCALAR) {
            min_indent_ = 0;
        }
    }

    void Scanner::update_indent()
    {
        ignore_tab_ = false;
        indent_ = col_ - tab_cnt_ - 1;
        if (next2_valid_) {
            indent_--;
        }
    }

    // 字符类型判断函数

#if 0
    // token解析函数
    // 解析数字类型，目前实现10进制int和real类型
    void Scanner::scan_number()
    {
        Token_Type token_type;

        while (is_number(next_char_)) {
            value_ += next_char();
        }

        // 判断是否为real
        if (next_char_ == '.') {
            token_type = Token_Type::REAL;
            value_ += next_char();
            while (is_number(next_char_)) {
                value_ += next_char();
            }
        } else {
            token_type = Token_Type::INT;
        }

        next_token_ = Token(token_type, value_);
    }
#endif

    void Scanner::scan_scalar()
    {
        if (!normal_ && (next_char_ == '|' || next_char_ == '>')) {
            scan_special_string();
        } else if (!normal_ && (next_char_ == '\'' || next_char_ == '\"')) {
            scan_quote_string();
        } else {
            scan_normal_string();
        }
    }

    void Scanner::scan_operator()
    {
        Token_Type token_type;
        char ch = next_char();
        value_ += ch;

        switch (ch) {
        case '-':
            if (next_char_ != '-') {
                token_type = Token_Type::SERIES_ENTRY;
            } else {
                // 读取后两个字符，判断是否匹配'---'
                value_ += next_char();
                ch = next_char();
                value_ += ch;
                if (ch == '-') {
                    token_type = Token_Type::START;
                } else {
                    scan_scalar();
                    return;
                }
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

        if (is_delimiter(next_char_))
            next_token_ = Token(token_type, value_, indent_);
        else
            scan_scalar();
    }

    void Scanner::scan_special_string()
    {
        assert(next_char_ == '|' || next_char_ == '>');

        Token_Type type = Token_Type::FOLD;
        // 换行替换字符
        if (next_char_ == '|') {
            type = Token_Type::PRESERVE;
            replace_ = '\n';
        }

        value_ += next_char();

        // 字符串末尾是否添加换行
        if (next_char_ == '-') {
            append_ = false;
            value_ += next_char();
        } else if (is_delimiter(next_char_) && next_char_ != -1) {
            append_ = true;
        } else {
            // 报错：
        }

        next_token_ = Token(type, value_, indent_);
    }

    void Scanner::scan_quote_string()
    {
        assert(next_char_ == '\'' || next_char_ == '\"');
        char end_char = next_char();
        String_Type string_type = end_char == '\''
                                          ? String_Type::SQUOTE_STRING
                                          : String_Type::DQUOTE_STRING;

        // 循环读取字符，直到 ’ 或 "
        while (next_char_ != end_char) {
            if (next_char_ == -1) {
                // TODO: 报错
            } else if (next_char_ == '\\' && end_char == '\"') {
                // 转义字符处理
                value_ += escape();
            } else if (next_char_ == '\n') {
                // 换行处理
                value_ += ' ';
                next_char();

                // 连续多个换行时不替换为空格
                while (next_char_ == '\n') {
                    value_ += '\n';
                    next_char();
                }
                while (next_char_ == ' ') {
                    next_char();
                }
            } else {
                value_ += next_char();
            }
        }

        // 消耗多出的引号
        next_char();

        next_token_ = Token(string_type, value_, indent_);
    }

    void Scanner::scan_normal_string(char replace, bool append_newline)
    {
        bool is_key = false;
        while (!input_end_ && col_ - tab_cnt_ - 1 >= min_indent_) {
            while (!input_end_ && next_char_ != '\n') {
                char ch = next_char();

                // 判断当前字符串属于 key 还是 value，如果是 key 则跳出
                if (ch == ':' && is_delimiter(next_char_)) {
                    next2_char_ = next_char_;
                    next_char_ = ch;
                    next2_valid_ = true;
                    is_key = true;
                    break;
                } else {
                    value_ += ch;
                }
            }

            if (is_key)
                break;

            // 消耗换行符
            if (next_char_ == '\n') {
                value_ += replace_;
                next_char();
                // 连续多个换行符不替换为空格
                while (next_char_ == '\n') {
                    value_ += '\n';
                    next_char();
                }
            }

            // 消耗下一行的空格
            while (!input_end_ && next_char_ == ' ') {
                next_char();
            }
        }

        value_ = value_.substr(0, value_.find_last_not_of(" \t\n\xFF") + 1);
        if (!value_.empty() && append_ && !is_key)
            value_ += '\n';

        replace_ = ' ';
        append_ = false;

        if (is_key)
            next_token_ = Token(Token_Type::KEY, value_, indent_);
        else {
            uint32_t temp_indent = value_.empty() ? indent_ + 2 : indent_;
            next_token_ =
                    Token(String_Type::NORMAL_STRING, value_, temp_indent);
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
            break;
        }

        // TODO: 报错 unknown escape sequence ...
        return -1;
    }

} // namespace cyaml
