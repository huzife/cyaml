/**
 * @file        parser.cpp
 * @brief       YAML 解析器词法分析部分 Parser 类源文件
 * @details     主要包含 YAML 的 Parser 类实现
 * @date        2023-7-25
 */

#include "parser/parser.h"
#include <assert.h>

namespace cyaml
{
    Parser::Parser(std::istream &in): scanner_(in) {}

} // namespace cyaml
