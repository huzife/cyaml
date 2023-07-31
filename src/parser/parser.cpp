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
        Node_Ptr node = std::make_shared<Node>("");

        ///< @todo  解析并构造数据树
        parse_stream(node);

        return Value(node);
    }

    Token Parser::expect(Token_Type type)
    {
        Token token = scanner_.lookahead();
        if (token.token_type() != type) {
            throw Parse_Exception(
                    unexpected_token_msg(type, token), scanner_.mark());
        }

        return scanner_.next_token();
    }

    void Parser::parse_stream(Node_Ptr &node)
    {
        expect(Token_Type::STREAM_START);
        parse_document(node);
        expect(Token_Type::STREAM_END);
    }

    void Parser::parse_document(Node_Ptr &node)
    {
        // DOC_START
        expect(Token_Type::DOC_START);

        // block_node
        parse_block_node(node);

        // DOC_END?
        if (scanner_.lookahead().token_type() == Token_Type::DOC_END)
            scanner_.next_token();
    }

    void Parser::parse_block_node(Node_Ptr &node)
    {
        Token next = scanner_.lookahead();
        switch (next.token_type()) {
        case Token_Type::BLOCK_MAP_START:
            parse_block_map(node);
            break;
        case Token_Type::BLOCK_SEQ_START:
            parse_block_sequence(node);
            break;
        case Token_Type::SCALAR:
            node = std::make_shared<Node>(next.value());
            scanner_.next_token();
            break;
        default:
            throw Parse_Exception(unexpected_token_msg(next), scanner_.mark());
        }
    }

    void Parser::parse_block_node_or_indentless_seq(Node_Ptr &node)
    {
        Token next = scanner_.lookahead();
        switch (next.token_type()) {
        case Token_Type::BLOCK_MAP_START:
            parse_block_map(node);
            break;
        case Token_Type::BLOCK_SEQ_START:
            parse_block_sequence(node);
            break;
        case Token_Type::BLOCK_SEQ_ENTRY:
            parse_indentless_block_sequence(node);
            break;
        case Token_Type::SCALAR:
            node = std::make_shared<Node>(next.value());
            scanner_.next_token();
            break;
        }
    }

    void Parser::parse_indentless_block_sequence(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::SEQUENCE);
        Token_Type type = scanner_.lookahead().token_type();
        while (type != Token_Type::KEY && type != Token_Type::BLOCK_MAP_END) {
            expect(Token_Type::BLOCK_SEQ_ENTRY);
            node->sequence_data_.emplace_back();
            parse_block_node(node->sequence_data_.back());
            type = scanner_.lookahead().token_type();
        }
    }

    void Parser::parse_block_map(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::MAP);
        expect(Token_Type::BLOCK_MAP_START);

        // 循环解析 key : value
        while (scanner_.lookahead().token_type() !=
               Token_Type::BLOCK_MAP_END) {
            Token key = expect(Token_Type::KEY);
            parse_block_node_or_indentless_seq(node->map_data_[key.value()]);
        }

        expect(Token_Type::BLOCK_MAP_END);
    }

    void Parser::parse_block_sequence(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::SEQUENCE);
        expect(Token_Type::BLOCK_SEQ_START);

        // 循环解析数组元素
        while (scanner_.lookahead().token_type() !=
               Token_Type::BLOCK_SEQ_END) {
            expect(Token_Type::BLOCK_SEQ_ENTRY);
            node->sequence_data_.emplace_back();
            parse_block_node(node->sequence_data_.back());
        }

        expect(Token_Type::BLOCK_SEQ_END);
    }

} // namespace cyaml
