/**
 * @file        parser.h
 * @brief       YAML 解析器语法分析部分 Parser 类头文件
 * @details     主要包含 YAML 的 Parser 类声明
 * @date        2023-7-25
 */

#ifndef CYAML_PARSER_H
#define CYAML_PARSER_H

#include "cyaml/event/event.h"
#include "cyaml/parser/scanner.h"
#include "cyaml/type/mark.h"
#include "cyaml/type/node/node.h"
#include "cyaml/type/node/node_impl.h"
#include "cyaml/type/tables.h"
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
        Event_Handler &handler_;

        mutable Mark mark_ = Mark(1, 1); // 当前 token 位置

    public:
        /**
         * @brief   Parser 类构造函数
         * @param   in          标准输入流
         * @param   handler     事件处理器
         */
        Parser(std::istream &in, Event_Handler &handler);

        /**
         * @brief   解析下一个 yaml 文档
         * @details 一个 yaml 文件可以存在多个文档，由 "---" 和 "..." 分隔
         * @return  bool
         */
        bool parse_next_document();

    private:
        /**
         * @brief   获取下一个 token
         * @return  Token
         */
        Token next_token()
        {
            Token ret = scanner_.next_token();
            mark_ = ret.mark();
            return ret;
        }

        /**
         * @brief   查看下一个 token
         * @return  Token
         */
        Token lookahead() const
        {
            Token ret = scanner_.lookahead();
            mark_ = ret.mark();
            return ret;
        }

        /**
         * @brief   获取当前 token 位置
         * @return  Mark
         */
        Mark mark() const
        {
            return mark_;
        }

        /**
         * @brief   尝试接收一个目标 token
         * @details 从 Scanner 中获取一个 token，并判断其类型是否符合传入类型
         * @return  Token
         */
        Token expect(Token_Type type);

        /**
         * @brief   判断下一个 token 是否属于某一个 first 集
         * @param   first_set       first 集
         * @return  bool
         */
        bool belong(const First_Set &first_set) const
        {
            auto type = next_type();
            return first_set.find(type) != first_set.end();
        }

        /**
         * @brief   belong 边界
         */
        constexpr bool belong() const
        {
            return false;
        }

        /**
         * @brief   判断下一个 token 是否属于某多个 first 集
         * @return  bool
         */
        template<typename... Args>
        bool belong(const First_Set &first_set, Args... sets) const
        {
            return belong(first_set) || belong(sets...);
        }

        /**
         * @brief   获取下个 token 类型
         * @return  Token_Type
         */
        Token_Type next_type() const
        {
            return lookahead().token_type();
        }

        /**
         * @brief   抛出错误 token 异常
         */
        void throw_unexpected_token();

        /**
         * @brief   抛出错误 token 异常
         * @param   expected_type   期望的 token 类型
         */
        void throw_unexpected_token(Token_Type expected_type);

        /**
         * @brief   插入键值对
         * @param   node        当前节点
         * @param   key_node    KEY 节点
         * @param   value_node  VALUE 节点
         * @return  void
         */
        void insert_key_value(
                Node_Ptr &node,
                Node_Ptr &key_node,
                Node_Ptr &value_node);

        // 递归下降解析函数
        void parse_document();
        void parse_block_node_or_indentless_seq();
        void parse_block_node();
        void parse_flow_node();
        void parse_block_content(std::string anchor);
        void parse_flow_content(std::string anchor);
        void parse_block_collection(std::string anchor);
        void parse_flow_collection(std::string anchor);
        void parse_block_map(std::string anchor);
        void parse_block_seq(std::string anchor);
        void parse_indentless_seq(std::string anchor);
        void parse_flow_map(std::string anchor);
        void parse_flow_seq(std::string anchor);
        void parse_flow_map_entry();
        void parse_flow_seq_entry();
        std::string parse_properties();
    };

} // namespace cyaml

#endif // CYAML_PARSER_H
