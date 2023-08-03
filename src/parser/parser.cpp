/**
 * @file        parser.cpp
 * @brief       YAML 解析器词法分析部分 Parser 类源文件
 * @details     主要包含 YAML 的 Parser 类实现
 * @date        2023-7-25
 */

#include "parser/parser.h"
#include "type/tables.h"
#include "error/exceptions.h"
#include <assert.h>
#include <iostream>

namespace cyaml
{
    Parser::Parser(std::istream &in): scanner_(in) {}

    Token Parser::expect(Token_Type type)
    {
        Token token = scanner_.lookahead();
        if (token.token_type() != type) {
            throw_unexpected_token(type);
        }

        return scanner_.next_token();
    }

    Value Parser::parse()
    {
        Node_Ptr node = std::make_shared<Node>("");

        parse_stream(node);

        return Value(node);
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
        if (belong(block_node_set))
            parse_block_node(node);

        // DOC_END?
        if (scanner_.lookahead().token_type() == Token_Type::DOC_END)
            scanner_.next_token();
    }

    void Parser::parse_block_node_or_indentless_seq(Node_Ptr &node)
    {
        if (belong(block_content_set))
            parse_block_content(node);
        else if (belong(indentless_seq_set))
            parse_indentless_seq(node);
        else
            throw_unexpected_token();
    }

    void Parser::parse_block_node(Node_Ptr &node)
    {
        parse_block_content(node);
    }

    void Parser::parse_block_content(Node_Ptr &node)
    {
        if (belong(block_collection_set))
            parse_block_collection(node);
        else if (scanner_.lookahead().token_type() == Token_Type::SCALAR) {
            Token next = scanner_.next_token();
            node = std::make_shared<Node>(next.value());
        } else
            throw_unexpected_token();
    }

    void Parser::parse_block_collection(Node_Ptr &node)
    {
        if (belong(block_map_set))
            parse_block_map(node);
        else if (belong(block_seq_set))
            parse_block_seq(node);
        else
            throw_unexpected_token();
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

    void Parser::parse_block_seq(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::SEQUENCE);
        expect(Token_Type::BLOCK_SEQ_START);

        // 循环解析数组元素
        while (scanner_.lookahead().token_type() !=
               Token_Type::BLOCK_SEQ_END) {
            expect(Token_Type::BLOCK_ENTRY);
            node->sequence_data_.emplace_back();
            parse_block_node(node->sequence_data_.back());
        }

        expect(Token_Type::BLOCK_SEQ_END);
    }

    void Parser::parse_indentless_seq(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::SEQUENCE);
        Token_Type type = scanner_.lookahead().token_type();
        while (type != Token_Type::KEY && type != Token_Type::BLOCK_MAP_END) {
            expect(Token_Type::BLOCK_ENTRY);
            node->sequence_data_.emplace_back();
            parse_block_node(node->sequence_data_.back());
            type = scanner_.lookahead().token_type();
        }
    }

} // namespace cyaml
