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
        if (next_type() != type) {
            throw_unexpected_token(type);
        }

        return next_token();
    }

    void Parser::throw_unexpected_token()
    {
        throw Parse_Exception(unexpected_token_msg(next_token()), mark());
    }

    void Parser::throw_unexpected_token(Token_Type expected_type)
    {
        throw Parse_Exception(
                unexpected_token_msg(expected_type, next_token()), mark());
    }

    void Parser::insert_key_value(
            Node_Ptr &node,
            Node_Ptr &key_node,
            Node_Ptr &value_node)
    {
        std::string key;
        Convert<std::string>::decode(*key_node, key);

        if (node->map_data_.find(key) != node_->map_data_.end()) {
            throw Representation_Exception(
                    std::string(error_msgs::DUPLICATED_KEY) + " '" + key + "'",
                    mark());
        }

        node->keys_.emplace_back(key_node);
        node->map_data_[key] = value_node;
    }

    // 解析部分
    Value Parser::parse()
    {
        parse_stream(node_);

        return Value(node_);
    }

    void Parser::parse_stream(Node_Ptr &node)
    {
        expect(Token_Type::STREAM_START);
        parse_document(node);
        expect(Token_Type::STREAM_END);
    }

    void Parser::parse_document(Node_Ptr &node)
    {
        // DOC_START?
        if (next_type() == Token_Type::DOC_START) {
            expect(Token_Type::DOC_START);
        }

        // block_node
        if (belong(block_node_set)) {
            parse_block_node(node);
        }

        // DOC_END?
        if (next_type() == Token_Type::DOC_END) {
            next_token();
        }
    }

    void Parser::parse_block_node_or_indentless_seq(Node_Ptr &node)
    {
        if (next_type() == Token_Type::ALIAS) {
            Token alias = next_token();
            if (anchor_map_.find(alias.value()) == anchor_map_.end()) {
                throw Parse_Exception(error_msgs::UNKNOWN_ANCHOR, mark());
            }
            node = anchor_map_[alias.value()];
        } else if (belong(properties_set, block_content_set,
                          indentless_seq_set)) {
            // 解析属性
            std::string anchor_name;
            if (belong(properties_set)) {
                anchor_name = parse_properties();
            }

            // 解析节点内容
            if (belong(block_content_set)) {
                parse_block_content(node);
            } else if (belong(indentless_seq_set)) {
                parse_indentless_seq(node);
            }

            // 插入 anchor 表
            if (!anchor_name.empty()) {
                anchor_map_[anchor_name] = node;
            }
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_block_node(Node_Ptr &node)
    {
        if (next_type() == Token_Type::ALIAS) {
            Token alias = next_token();
            if (anchor_map_.find(alias.value()) == anchor_map_.end()) {
                throw Parse_Exception(error_msgs::UNKNOWN_ANCHOR, mark());
            }
            node = anchor_map_[alias.value()];
        } else if (belong(properties_set, block_content_set)) {
            // 解析属性
            std::string anchor_name;
            if (belong(properties_set)) {
                anchor_name = parse_properties();
            }

            // 解析节点内容
            if (belong(block_content_set)) {
                parse_block_content(node);
            }

            // 插入 anchor 表
            if (!anchor_name.empty()) {
                anchor_map_[anchor_name] = node;
            }
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_flow_node(Node_Ptr &node)
    {
        if (next_type() == Token_Type::ALIAS) {
            Token alias = next_token();
            if (anchor_map_.find(alias.value()) == anchor_map_.end()) {
                throw Parse_Exception(error_msgs::UNKNOWN_ANCHOR, mark());
            }
            node = anchor_map_[alias.value()];
        } else if (belong(properties_set, flow_content_set)) {
            // 解析属性
            std::string anchor_name;
            if (belong(properties_set)) {
                anchor_name = parse_properties();
            }

            // 解析节点内容
            if (belong(flow_content_set)) {
                parse_flow_content(node);
            }

            // 插入 anchor 表
            if (!anchor_name.empty()) {
                anchor_map_[anchor_name] = node;
            }
        } else {
            throw_unexpected_token();
        }
    }

    std::string Parser::parse_properties()
    {
        // 目前只有 anchor，未实现 tag
        Token anchor = expect(Token_Type::ANCHOR);
        return anchor.value();
    }

    void Parser::parse_block_content(Node_Ptr &node)
    {
        if (belong(block_collection_set)) {
            parse_block_collection(node);
        } else if (belong(flow_collection_set)) {
            parse_flow_collection(node);
        } else if (next_type() == Token_Type::SCALAR) {
            node = std::make_shared<Node>(next_token().value());
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_flow_content(Node_Ptr &node)
    {
        if (belong(flow_collection_set)) {
            parse_flow_collection(node);
        } else if (next_type() == Token_Type::SCALAR) {
            node = std::make_shared<Node>(next_token().value());
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_block_collection(Node_Ptr &node)
    {
        if (belong(block_map_set)) {
            parse_block_map(node);
        } else if (belong(block_seq_set)) {
            parse_block_seq(node);
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_flow_collection(Node_Ptr &node)
    {
        if (belong(flow_map_set)) {
            parse_flow_map(node);
        } else if (belong(flow_seq_set)) {
            parse_flow_seq(node);
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_block_map(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::MAP);
        expect(Token_Type::BLOCK_MAP_START);

        // 循环解析 key : value
        while (next_type() != Token_Type::BLOCK_MAP_END) {
            // key 部分，默认为 "null"
            auto key_node = std::make_shared<Node>("null");
            if (next_type() == Token_Type::KEY) {
                next_token();
                if (belong(block_node_or_indentless_seq_set)) {
                    parse_block_node_or_indentless_seq(key_node);
                }
            }

            // value 部分，默认为 null
            auto value_node = std::make_shared<Node>();
            if (next_type() == Token_Type::VALUE) {
                next_token();
                if (belong(block_node_or_indentless_seq_set)) {
                    parse_block_node_or_indentless_seq(value_node);
                }
            }

            // 插入键值对
            insert_key_value(node, key_node, value_node);
        }

        expect(Token_Type::BLOCK_MAP_END);
    }

    void Parser::parse_block_seq(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::SEQ);
        expect(Token_Type::BLOCK_SEQ_START);

        // 循环解析数组元素
        while (next_type() != Token_Type::BLOCK_SEQ_END) {
            expect(Token_Type::BLOCK_ENTRY);
            auto value_node = std::make_shared<Node>();
            if (belong(block_node_set)) {
                parse_block_node(value_node);
            }
            node->seq_data_.emplace_back(value_node);
        }

        expect(Token_Type::BLOCK_SEQ_END);
    }

    void Parser::parse_indentless_seq(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::SEQ);
        Token_Type type = next_type();

        // 至少一次
        expect(Token_Type::BLOCK_ENTRY);
        auto value_node = std::make_shared<Node>();
        if (belong(block_node_set)) {
            parse_block_node(value_node);
        }
        node->seq_data_.emplace_back(value_node);

        while (next_type() != Token_Type::KEY &&
               next_type() != Token_Type::BLOCK_MAP_END) {
            expect(Token_Type::BLOCK_ENTRY);
            value_node = std::make_shared<Node>();
            if (belong(block_node_set)) {
                parse_block_node(value_node);
            }
            node->seq_data_.emplace_back(value_node);
        }
    }

    void Parser::parse_flow_map(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::MAP);
        expect(Token_Type::FLOW_MAP_START);

        // 循环解析 key : value
        while (next_type() != Token_Type::FLOW_MAP_END) {
            auto key_node = std::make_shared<Node>("null");
            auto value_node = std::make_shared<Node>();

            if (belong(flow_map_entry_set)) {
                parse_flow_map_entry(key_node, value_node);
            }

            // 插入键值对
            insert_key_value(node, key_node, value_node);

            if (next_type() != Token_Type::FLOW_MAP_END) {
                expect(Token_Type::FLOW_ENTRY);
            }
        }

        expect(Token_Type::FLOW_MAP_END);
    }

    void Parser::parse_flow_seq(Node_Ptr &node)
    {
        node = std::make_shared<Node>(Node_Type::SEQ);
        expect(Token_Type::FLOW_SEQ_START);

        // 循环解析数组元素
        while (next_type() != Token_Type::FLOW_SEQ_END) {
            auto value_node = std::make_shared<Node>();
            if (belong(flow_seq_entry_set)) {
                parse_flow_seq_entry(value_node);
            }
            node->seq_data_.emplace_back(value_node);

            if (next_type() != Token_Type::FLOW_SEQ_END) {
                expect(Token_Type::FLOW_ENTRY);
            }
        }

        expect(Token_Type::FLOW_SEQ_END);
    }

    void Parser::parse_flow_map_entry(Node_Ptr &key_node, Node_Ptr &value_node)
    {
        if (belong(flow_node_set)) {
            parse_flow_node(key_node);
        } else if (next_type() == Token_Type::KEY) {
            next_token();
            if (belong(flow_node_set)) {
                parse_flow_node(key_node);
            }

            if (next_type() == Token_Type::VALUE) {
                next_token();
                if (belong(flow_node_set)) {
                    parse_flow_node(value_node);
                }
            }
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_flow_seq_entry(Node_Ptr &node)
    {
        if (belong(flow_node_set)) {
            parse_flow_node(node);
        } else if (next_type() == Token_Type::KEY) {
            next_token();
            // [] 中的键值对需要单独放到一个 {} 中
            node = std::make_shared<Node>(Node_Type::MAP);

            auto key_node = std::make_shared<Node>("null");
            if (belong(flow_node_set)) {
                parse_flow_node(key_node);
            }

            auto value_node = std::make_shared<Node>();
            if (next_type() == Token_Type::VALUE) {
                next_token();
                if (belong(flow_node_set)) {
                    parse_flow_node(value_node);
                }
            }

            // 插入键值对
            insert_key_value(node, key_node, value_node);
        } else {
            throw_unexpected_token();
        }
    }

} // namespace cyaml
