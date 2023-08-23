/**
 * @file        parser.cpp
 * @brief       YAML 解析器词法分析部分 Parser 类源文件
 * @details     主要包含 YAML 的 Parser 类实现
 * @date        2023-7-25
 */

#include "cyaml/parser/parser.h"
#include "cyaml/type/tables.h"
#include "cyaml/error/exceptions.h"
#include <assert.h>
#include <iostream>

namespace cyaml
{
    Parser::Parser(std::istream &in, Event_Handler &handler)
        : scanner_(in),
          handler_(handler)
    {
    }

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

    // 解析部分
    bool Parser::parse_next_document()
    {
        if (scanner_.end())
            return false;

        parse_document();
        return true;
    }

    void Parser::parse_document()
    {
        // DOC_START?
        if (next_type() == Token_Type::DOC_START) {
            next_token();
        }
        handler_.on_document_start(mark());

        // block_node
        if (belong(block_node_set)) {
            parse_block_node();
        } else {
            handler_.on_null(mark(), "");
        }

        // DOC_END*
        while (next_type() == Token_Type::DOC_END) {
            next_token();
        }
        handler_.on_document_end();
    }

    void Parser::parse_block_node_or_indentless_seq()
    {
        if (next_type() == Token_Type::ALIAS) {
            Token alias = next_token();
            handler_.on_alias(mark(), alias.value());
        } else if (belong(properties_set, block_content_set,
                          indentless_seq_set)) {
            // 解析属性
            std::string anchor_name;
            if (belong(properties_set)) {
                anchor_name = parse_properties();
            }

            // 解析节点内容
            if (belong(block_content_set)) {
                parse_block_content(anchor_name);
            } else if (belong(indentless_seq_set)) {
                parse_indentless_seq(anchor_name);
            }
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_block_node()
    {
        if (next_type() == Token_Type::ALIAS) {
            Token alias = next_token();
            handler_.on_alias(mark(), alias.value());
        } else if (belong(properties_set, block_content_set)) {
            // 解析属性
            std::string anchor_name;
            if (belong(properties_set)) {
                anchor_name = parse_properties();
            }

            // 解析节点内容
            if (belong(block_content_set)) {
                parse_block_content(anchor_name);
            }
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_flow_node()
    {
        if (next_type() == Token_Type::ALIAS) {
            Token alias = next_token();
            handler_.on_alias(mark(), alias.value());
        } else if (belong(properties_set, flow_content_set)) {
            // 解析属性
            std::string anchor_name;
            if (belong(properties_set)) {
                anchor_name = parse_properties();
            }

            // 解析节点内容
            if (belong(flow_content_set)) {
                parse_flow_content(anchor_name);
            }
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_block_content(std::string anchor)
    {
        if (belong(block_collection_set)) {
            parse_block_collection(anchor);
        } else if (belong(flow_collection_set)) {
            parse_flow_collection(anchor);
        } else if (next_type() == Token_Type::SCALAR) {
            handler_.on_scalar(mark(), anchor, next_token().value());
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_flow_content(std::string anchor)
    {
        if (belong(flow_collection_set)) {
            parse_flow_collection(anchor);
        } else if (next_type() == Token_Type::SCALAR) {
            handler_.on_scalar(mark(), anchor, next_token().value());
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_block_collection(std::string anchor)
    {
        if (belong(block_map_set)) {
            parse_block_map(anchor);
        } else if (belong(block_seq_set)) {
            parse_block_seq(anchor);
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_flow_collection(std::string anchor)
    {
        if (belong(flow_map_set)) {
            parse_flow_map(anchor);
        } else if (belong(flow_seq_set)) {
            parse_flow_seq(anchor);
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_block_map(std::string anchor)
    {
        expect(Token_Type::BLOCK_MAP_START);
        handler_.on_map_start(mark(), anchor, Node_Style::BLOCK);

        // 循环解析 key : value
        while (next_type() != Token_Type::BLOCK_MAP_END) {
            // key 部分，默认为 "null"
            bool null = true;
            if (next_type() == Token_Type::KEY) {
                next_token();
                if (belong(block_node_or_indentless_seq_set)) {
                    parse_block_node_or_indentless_seq();
                    null = false;
                }
            }

            if (null) {
                handler_.on_null(mark(), "");
            }

            // value 部分，默认为 null
            null = true;
            if (next_type() == Token_Type::VALUE) {
                next_token();
                if (belong(block_node_or_indentless_seq_set)) {
                    parse_block_node_or_indentless_seq();
                    null = false;
                }
            }

            if (null) {
                handler_.on_null(mark(), "");
            }
        }

        expect(Token_Type::BLOCK_MAP_END);
        handler_.on_map_end();
    }

    void Parser::parse_block_seq(std::string anchor)
    {
        expect(Token_Type::BLOCK_SEQ_START);
        handler_.on_seq_start(mark(), anchor, Node_Style::BLOCK);

        // 循环解析数组元素
        while (next_type() != Token_Type::BLOCK_SEQ_END) {
            expect(Token_Type::BLOCK_ENTRY);
            if (belong(block_node_set)) {
                parse_block_node();
            } else {
                handler_.on_null(mark(), "");
            }
        }

        expect(Token_Type::BLOCK_SEQ_END);
        handler_.on_seq_end();
    }

    void Parser::parse_indentless_seq(std::string anchor)
    {
        Token_Type type = next_type();
        handler_.on_seq_start(mark(), anchor, Node_Style::BLOCK);

        // 至少一次
        do {
            expect(Token_Type::BLOCK_ENTRY);
            if (belong(block_node_set)) {
                parse_block_node();
            } else {
                handler_.on_null(mark(), "");
            }
        } while (next_type() != Token_Type::KEY &&
                 next_type() != Token_Type::BLOCK_MAP_END);

        handler_.on_seq_end();
    }

    void Parser::parse_flow_map(std::string anchor)
    {
        expect(Token_Type::FLOW_MAP_START);
        handler_.on_map_start(mark(), anchor, Node_Style::FLOW);

        // 循环解析 key : value
        while (next_type() != Token_Type::FLOW_MAP_END) {
            if (belong(flow_map_entry_set)) {
                parse_flow_map_entry();
            } else {
                handler_.on_null(mark(), "");
                handler_.on_null(mark(), "");
            }

            if (next_type() != Token_Type::FLOW_MAP_END) {
                expect(Token_Type::FLOW_ENTRY);
            }
        }

        expect(Token_Type::FLOW_MAP_END);
        handler_.on_map_end();
    }

    void Parser::parse_flow_seq(std::string anchor)
    {
        expect(Token_Type::FLOW_SEQ_START);
        handler_.on_seq_start(mark(), anchor, Node_Style::FLOW);

        // 循环解析数组元素
        while (next_type() != Token_Type::FLOW_SEQ_END) {
            if (belong(flow_seq_entry_set)) {
                parse_flow_seq_entry();
            } else {
                handler_.on_null(mark(), "");
            }

            if (next_type() != Token_Type::FLOW_SEQ_END) {
                expect(Token_Type::FLOW_ENTRY);
            }
        }

        expect(Token_Type::FLOW_SEQ_END);
        handler_.on_seq_end();
    }

    void Parser::parse_flow_map_entry()
    {
        if (belong(flow_node_set)) {
            parse_flow_node();
            handler_.on_null(mark(), "");
        } else if (next_type() == Token_Type::KEY) {
            next_token();
            if (belong(flow_node_set)) {
                parse_flow_node();
            } else {
                handler_.on_null(mark(), "");
            }

            bool null = true;
            if (next_type() == Token_Type::VALUE) {
                next_token();
                if (belong(flow_node_set)) {
                    parse_flow_node();
                    null = false;
                }
            }
            if (null) {
                handler_.on_null(mark(), "");
            }
        } else {
            throw_unexpected_token();
        }
    }

    void Parser::parse_flow_seq_entry()
    {
        if (belong(flow_node_set)) {
            parse_flow_node();
        } else if (next_type() == Token_Type::KEY) {
            next_token();

            // [] 中的键值对需要单独放到一个 {} 中
            handler_.on_map_start(mark(), "", Node_Style::FLOW);

            if (belong(flow_node_set)) {
                parse_flow_node();
            } else {
                handler_.on_null(mark(), "");
            }

            bool null = true;
            if (next_type() == Token_Type::VALUE) {
                next_token();
                if (belong(flow_node_set)) {
                    parse_flow_node();
                    null = false;
                }
            }
            if (null) {
                handler_.on_null(mark(), "");
            }

            handler_.on_map_end();
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

} // namespace cyaml
