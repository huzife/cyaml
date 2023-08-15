/**
 * @file        token.cpp
 * @brief       YAML 解析器 Token 类源文件
 * @details     主要包含 YAML 的 Token 类实现
 * @date        2023-7-20
 */

#include "type/token.h"
#include <assert.h>

namespace cyaml
{
    Token::Token(): token_type_(Token_Type::NONE) {}

    Token::Token(Token_Type type, Mark mark): token_type_(type), mark_(mark) {}

    Token::Token(Token_Type type, std::string value, Mark mark)
        : token_type_(type),
          value_(value),
          mark_(mark),
          has_value_(true)
    {
    }

    std::string Token::to_string() const
    {
        std::string ret = "(" + token_type_to_string(token_type_);
        if (has_value()) {
            ret += (", " + value_);
        }
        ret += ")";
        return ret;
    }

    std::ostream &operator<<(std::ostream &out, const Token &token)
    {
        out << token.to_string();
        return out;
    }

    std::string token_type_to_string(Token_Type type)
    {
        switch (type) {
        case Token_Type::NONE:
            return "NONE";
        case Token_Type::DOC_START:
            return "DOC_START";
        case Token_Type::DOC_END:
            return "DOC_END";
        case Token_Type::BLOCK_ENTRY:
            return "BLOCK_ENTRY";
        case Token_Type::BLOCK_MAP_START:
            return "BLOCK_MAP_START";
        case Token_Type::BLOCK_MAP_END:
            return "BLOCK_MAP_END";
        case Token_Type::BLOCK_SEQ_START:
            return "BLOCK_SEQ_START";
        case Token_Type::BLOCK_SEQ_END:
            return "BLOCK_SEQ_END";
        case Token_Type::FLOW_ENTRY:
            return "FLOW_ENTRY";
        case Token_Type::FLOW_MAP_START:
            return "FLOW_MAP_START";
        case Token_Type::FLOW_MAP_END:
            return "FLOW_MAP_END";
        case Token_Type::FLOW_SEQ_START:
            return "FLOW_SEQ_START";
        case Token_Type::FLOW_SEQ_END:
            return "FLOW_SEQ_END";
        case Token_Type::KEY:
            return "KEY";
        case Token_Type::VALUE:
            return "VALUE";
        case Token_Type::SCALAR:
            return "SCALAR";
        case Token_Type::ANCHOR:
            return "ANCHOR";
        case Token_Type::ALIAS:
            return "ALIAS";
        }

        // unreachabled
        assert(false);
        return "";
    }

    Token_Type from_indent_type(Indent_Type type, bool is_start)
    {
        assert(type != Indent_Type::NONE);
        if (type == Indent_Type::MAP)
            return is_start ? Token_Type::BLOCK_MAP_START
                            : Token_Type::BLOCK_MAP_END;
        else
            return is_start ? Token_Type::BLOCK_SEQ_START
                            : Token_Type::BLOCK_SEQ_END;
    }

    Token_Type from_flow_type(Flow_Type type, bool is_start)
    {
        assert(type != Flow_Type::NONE);
        if (type == Flow_Type::MAP)
            return is_start ? Token_Type::FLOW_MAP_START
                            : Token_Type::FLOW_MAP_END;
        else
            return is_start ? Token_Type::FLOW_SEQ_START
                            : Token_Type::FLOW_SEQ_END;
    }

} // namespace cyaml
