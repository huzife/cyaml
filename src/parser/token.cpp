/**
 * @file        token.cpp
 * @brief       YAML 解析器 Token 类源文件
 * @details     主要包含 YAML 的 Token 类实现
 * @date        2023-7-20
 */

#include "parser/token.h"

namespace cyaml
{
    Token::Token(): token_type_(Token_Type::NONE) {}

    Token::Token(Token_Type type, std::string value, uint32_t indent)
        : token_type_(type), value_(value), indent_(indent)
    {
    }

    Token::Token(String_Type str_type, std::string value, uint32_t indent)
        : token_type_(Token_Type::SCALAR),
          value_(value),
          string_type_(str_type),
          indent_(indent)
    {
    }

    std::string Token::to_string() const
    {
        return "(" + token_type_to_string(token_type_) + ", " + value_ + ", " +
               std::to_string(indent_) + ")";
    }

    std::ostream &operator<<(std::ostream &out, const Token &token)
    {
        out << token.to_string();
        return out;
    }

    static std::string token_type_to_string(Token_Type type)
    {
        switch (type) {
        case Token_Type::NONE:
            return "NONE";
        case Token_Type::START:
            return "START";
        case Token_Type::END:
            return "END";
        case Token_Type::SERIES_ENTRY:
            return "SERIES_ENTRY";
        case Token_Type::COLON:
            return "COLON";
        case Token_Type::LBRACE:
            return "LBRACE";
        case Token_Type::RBRACE:
            return "RBRACE";
        case Token_Type::LBRACKET:
            return "LBRACKET";
        case Token_Type::RBRACKET:
            return "RBRACKET";
        case Token_Type::PRESERVE:
            return "PRESERVE";
        case Token_Type::FOLD:
            return "FOLD";
        case Token_Type::SQUOTE:
            return "SQUOTE";
        case Token_Type::DQUOTE:
            return "DQUOTE";
        case Token_Type::KEY:
            return "KEY";
        case Token_Type::SCALAR:
            return "SCALAR";
        }

        // unreachabled
        assert(false);
        return "";
    }

} // namespace cyaml
