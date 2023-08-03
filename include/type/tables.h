/**
 * @file    tables.h
 * @brief   包含解析过程中使用的分析表
 * @details 定义了转义字符表、first 集查询表
 * @date    2023-8-23
 */

#ifndef CYAML_TABLES_H
#define CYAML_TABLES_H

#include "type/token.h"
#include <unordered_map>
#include <unordered_set>

namespace cyaml
{
    static const std::unordered_map<char, char> escape_map{
            {'a', '\a'},  {'b', '\b'},  {'f', '\f'},  {'n', '\n'},
            {'r', '\r'},  {'t', '\t'},  {'v', '\v'},  {'e', '\e'},
            {'\\', '\\'}, {'\"', '\"'}, {'\'', '\''}, {'0', '\0'}};

    using First_Set = std::unordered_set<Token_Type>;

    static const First_Set stream_set = {Token_Type::STREAM_START};

    static const First_Set document_set = {
            Token_Type::SCALAR, Token_Type::BLOCK_MAP_START,
            Token_Type::BLOCK_SEQ_START, Token_Type::FLOW_MAP_START,
            Token_Type::FLOW_SEQ_START};

    static const First_Set block_node_or_indentless_seq_set = {
            Token_Type::SCALAR,          Token_Type::BLOCK_MAP_START,
            Token_Type::BLOCK_SEQ_START, Token_Type::FLOW_MAP_START,
            Token_Type::FLOW_SEQ_START,  Token_Type::BLOCK_ENTRY};

    static const First_Set block_node_set = {
            Token_Type::SCALAR, Token_Type::BLOCK_MAP_START,
            Token_Type::BLOCK_SEQ_START, Token_Type::FLOW_MAP_START,
            Token_Type::FLOW_SEQ_START};

    static const First_Set block_content_set = {
            Token_Type::SCALAR, Token_Type::BLOCK_MAP_START,
            Token_Type::BLOCK_SEQ_START, Token_Type::FLOW_MAP_START,
            Token_Type::FLOW_SEQ_START};

    static const First_Set block_collection_set = {
            Token_Type::BLOCK_MAP_START, Token_Type::BLOCK_SEQ_START};

    static const First_Set block_map_set = {Token_Type::BLOCK_MAP_START};

    static const First_Set block_seq_set = {Token_Type::BLOCK_SEQ_START};

    static const First_Set indentless_seq_set = {Token_Type::BLOCK_ENTRY};

} // namespace cyaml

#endif // CYAML_TABLES_H
