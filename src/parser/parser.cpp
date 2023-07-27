/**
 * @file        parser.cpp
 * @brief       YAML 解析器词法分析部分 Parser 类源文件
 * @details     主要包含 YAML 的 Parser 类实现
 * @date        2023-7-25
 */

#include "parser/parser.h"
#include "error/exceptions.h"
#include <assert.h>
#include <iostream>

namespace cyaml
{
    Parser::Parser(std::istream &in): scanner_(in) {}

    Value Parser::parse()
    {
        Node_Ptr node = std::make_shared<Node>();

        ///< @todo  解析并构造数据树
        parse_stream(node);

        return Value(node);
    }

    void Parser::parse_stream(Node_Ptr &node)
    {
        Token token = scanner_.next_token();
        assert(token.token_type() == Token_Type::START);

        token = scanner_.lookahead();
        if (token.token_type() == Token_Type::SCALAR) {
            token = scanner_.next_token();
            node = std::make_shared<Node>(token.string_value());
        } else if (token.token_type() == Token_Type::DASH) {
            node = std::make_shared<Node>(Data_Type::SEQUENCE);
            ///< @todo 解析序列(数组)

        } else if (token.token_type() == Token_Type::KEY) {
            node = std::make_shared<Node>(Data_Type::MAPPING);
            ///< @todo 解析映射(对象)

        } else {
            ///< @todo 错误处理
        }

        token = scanner_.next_token();
        assert(token.token_type() == Token_Type::NONE ||
               token.token_type() == Token_Type::END);
    }

} // namespace cyaml
