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
#include "type/tables.h"
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
        /**
         * @brief   尝试接收一个目标 token
         * @details 从 Scanner 中获取一个 token，并判断其类型是否符合传入类型
         * @return  Token
         * @retval  类型匹配时，返回接收到的 token
         */
        Token expect(Token_Type type);

        /**
         * @brief   判断下一个字符是否属于某个 first 集
         * @param   const First_Set &
         * @return  bool
         */
        bool belong(const First_Set &first_set) const
        {
            auto type = scanner_.lookahead().token_type();
            return first_set.find(type) != first_set.end();
        }

        /**
         * @brief   抛出错误 token 异常
         */
        void throw_unexpected_token()
        {
            Token next = scanner_.next_token();
            Mark mark = scanner_.mark();
            throw Parse_Exception(unexpected_token_msg(next), mark);
        }

        /**
         * @brief   抛出错误 token 异常
         * @param   Token_Type      期望的 token 类型
         */
        void throw_unexpected_token(Token_Type expected_type)
        {
            Token next = scanner_.next_token();
            Mark mark = scanner_.mark();
            throw Parse_Exception(
                    unexpected_token_msg(expected_type, next), mark);
        }

        void parse_stream(Node_Ptr &node);
        void parse_document(Node_Ptr &node);
        void parse_block_node_or_indentless_seq(Node_Ptr &node);
        void parse_block_node(Node_Ptr &node);
        void parse_block_content(Node_Ptr &node);
        void parse_block_collection(Node_Ptr &node);
        void parse_block_map(Node_Ptr &node);
        void parse_block_seq(Node_Ptr &node);
        void parse_indentless_seq(Node_Ptr &node);
    };

} // namespace cyaml

#endif // CYAML_PARSER_H
