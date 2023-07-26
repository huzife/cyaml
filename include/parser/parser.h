/**
 * @file        parser.h
 * @brief       YAML 解析器词法分析部分 Parser 类头文件
 * @details     主要包含 YAML 的 Parser 类声明
 * @date        2023-7-25
 */

#ifndef CYAML_PARSER_H
#define CYAML_PARSER_H

#include "parser/scanner.h"
#include "type/node.h"
#include "type/value.h"
#include <unordered_map>
#include <unordered_set>

namespace cyaml
{
    /**
     * @class   Parser
     * @brief   YAML 语法分析器
     */
    class Parser
    {
    private:
        Scanner scanner_;

    public:
        /**
         * @brief   Parser 类构造函数
         * @param   std::istream    标准输入流
         * @retval  Parser 对象
         */
        Parser(std::istream &in);

        /**
         * @brief   YAML 解析接口
         * @details 使用递归下降解析 YAML 文本流
         * @return  Value
         * @retval  YAML 解析得到的数据对象
         */
        Value parse();

    private:
        void parse_stream(Node_Ptr &node);
        void parse_sequence(Node_Ptr &node);
        void parse_mapping(Node_Ptr &node);
    };

} // namespace cyaml

#endif // CYAML_PARSER_H
