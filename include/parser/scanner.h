/**
 * @file        scanner.h
 * @brief       YAML 解析器词法分析部分 Scanner 类头文件
 * @details     主要包含 YAML 的 Scanner 类声明
 * @date        2023-7-20
 */

#ifndef CYAML_SCANNER_H
#define CYAML_SCANNER_H

#include "type/token.h"
#include "type/mark.h"
#include "type/indent.h"
#include "parser/stream.h"
#include <istream>
#include <stack>
#include <queue>
#include <iostream>

namespace cyaml
{
    /**
     * @enum    Match_End
     * @brief   匹配字符串结束字符
     */
    enum class Match_End
    {
        ANY,
        BLANK
    };

    /**
     * @class   Scanner
     * @brief   YAML 词法分析器
     */
    class Scanner
    {
    private:
        Stream input_;

        Mark token_mark_ = Mark(1, 1); // 当前 token 位置
        uint32_t tab_cnt_ = 0;         // 该行'\t'个数
        uint32_t cur_indent_ = 0;      // 当前 token 的缩进长度
        uint32_t min_indent_ = 0;      // 用于限制多行字符串缩进
        bool ignore_tab_ = true; // 是否忽略 '\t'，用于计算缩进
        bool scan_end_ = false;  // 记录是否解析完所有 token

        std::stack<Indent> indent_;  // 缩进状态栈
        std::stack<Flow_Type> flow_; // 流状态栈

        std::queue<Token> token_; // 暂存下一个 token

        char replace_ = ' ';      // 字符串换行时替换的字符
        bool append_ = false;     // 字符串末尾是否添加换行
        bool in_special_ = false; // 是否正在扫描特殊字符串

        uint32_t anchor_indent_ = 0; // 锚点缩进
        bool after_anchor_ = false;  // 是否位于同一行的锚点后

        bool can_be_json = false; // 判断能否作为 json

    public:
        /**
         * @brief   Scanner 类构造函数
         * @param   in      输入流
         */
        Scanner(std::istream &in);

        /**
         * @brief   获取下一个 token
         * @details 从输入流扫描并解析出下一个 token
         * @return  Token
         * @retval  Token(NONE):    表示没有下一个 token
         */
        Token next_token();

        /**
         * @brief   查看但不消耗下一个 token
         * @details 该解析器通过基于 YAML 的 LL(1)文法实现，
         *          需要向前查看一个符号而不消耗这个 token
         * @return  Token
         * @retval  Token(NONE):    表示没有下一个 token
         */
        Token lookahead() const;

        /**
         * @brief   返回当前正在扫描 token 位置
         * @return  Mark
         */
        Mark token_mark() const
        {
            return token_mark_;
        }

        /**
         * @brief   获取当前缩进位置
         * @return  uint32_t
         */
        uint32_t get_cur_indent() const
        {
            return input_.column() - tab_cnt_ - 1;
        }

        /**
         * @brief   返回 Scanner 扫描状态
         * @return  bool
         * @retval  true:   扫描结束，无后续 token
         * @retval  false:  扫描未结束，还有未解析 token
         */
        bool end()
        {
            return scan_end_ && token_.empty();
        }

    private:
        /**
         * @brief   读取下一个字符
         * @return  char
         */
        char next_char();

        /**
         * @brief   添加 token
         * @tparam  Args...     Token 构造参数
         * @return  void
         */
        template<typename... Args>
        void add_token(Args... args)
        {
            Token t(args..., token_mark_);
            token_.push(t);
        }

        /**
         * @brief   更新缩进值
         * @return  void
         */
        void update_indent();

        /**
         * @brief   跳过空白字符直到下一个 token
         * @return  void
         */
        void skip_to_next_token();

        /**
         * @brief   跳过空白符号
         * @return  void
         */
        void skip_blank()
        {
            while (input_ && is_delimiter(input_.peek())) {
                next_char();
            }
        }

        /**
         * @brief   跳过注释
         * @return  void
         */
        void skip_comment()
        {
            if (input_.peek() == '#') {
                while (input_ && input_.peek() != '\n') {
                    next_char();
                }
            }
        }

        /**
         * @brief   重置标量解析标志
         * @return  void
         */
        void reset_scalar_flags()
        {
            replace_ = ' ';
            append_ = false;
            in_special_ = false;
        }

        /**
         * @brief   扫描并解析一个 token
         * @return  void
         */
        void scan();

        /**
         * @brief   结束 yaml 流
         * @return  void
         */
        void stream_end();

        /**
         * @brief   扫描 DOC_START
         * @return  void
         */
        void scan_doc_start();

        /**
         * @brief   扫描 DOC_END
         * @return  void
         */
        void scan_doc_end();

        /**
         * @brief   扫描 ANCHOR
         * @return  void
         */
        void scan_anchor();

        /**
         * @brief   扫描 ALIAS
         * @return  void
         */
        void scan_alias();

        /**
         * @brief   扫描 FLOW_MAP_START 或 FLOW_SEQ_START
         * @return  void
         */
        void scan_flow_start();

        /**
         * @brief   扫描 FLOW_MAP_END 或 FLOW_SEQ_END
         * @return  void
         */
        void scan_flow_end();

        /**
         * @brief   扫描 FLOW_ENTRY
         * @return  void
         */
        void scan_flow_entry();

        /**
         * @brief   扫描 BLOCK_ENTRY
         * @return  void
         */
        void scan_block_entry();

        /**
         * @brief   扫描 KEY
         * @return  void
         */
        void scan_key();

        /**
         * @brief   扫描 value
         * @return  void
         */
        void scan_value();

        /**
         * @brief   扫描特殊字符串，包括保留换行和折叠换行
         * @details 该函数实际上只是识别保留换行和折叠换行相关符号，
         *          然后调用 scan_normal_scalar()
         * @return  void
         */
        void scan_special_scalar();

        /**
         * @brief   扫描带引号的两种字符串
         * @return  void
         */
        void scan_quote_scalar();

        /**
         * @brief   扫描字符串
         * @details 结合缩进检测字符串边界
         * @return  void
         */
        void scan_normal_scalar();

        /**
         * @brief   扫描到 KEY 或 '-' 后调用
         * @return  void
         */
        void start_scalar()
        {
            min_indent_ = cur_indent_ + 1;
        }

        /**
         * @brief   扫描标量结束后调用
         * @return  void
         */
        void end_scalar()
        {
            min_indent_ = 0;
        }

        /**
         * @brief   处理转义字符
         * @return  char
         */
        char escape();

        /**
         * @brief   推入缩进值
         * @param   type    缩进类型
         */
        void push_indent(Indent_Type type);

        /**
         * @brief   弹出缩进值
         * @return  void
         */
        void pop_indent();

        /**
         * @brief   匹配剩余所有缩进
         * @return  void
         */
        void pop_all_indent()
        {
            while (!indent_.empty()) {
                auto type = indent_.top().type;
                add_token(from_indent_type(type, Collection_Flag::END));
                indent_.pop();
            }
        }

        /**
         * @brief   判断当前是否处于块节点内
         * @return  bool
         */
        bool in_block() const
        {
            return flow_.empty();
        }

        /**
         * @brief   判断是否特殊标量
         * @return  bool
         */
        bool in_special() const
        {
            assert(in_block() || !in_special_);
            return in_special_;
        }

        /**
         * @brief   判断是否处于 flow_map
         * @return  bool
         */
        bool in_flow_map() const
        {
            return !in_block() && flow_.top() == Flow_Type::MAP;
        }

        /**
         * @brief   判断是否处于 flow_seq
         * @return  bool
         */
        bool in_flow_seq() const
        {
            return !in_block() && flow_.top() == Flow_Type::SEQ;
        }

        /**
         * @brief   判断字符是否属于分隔符
         * @param   ch      判断字符
         * @return  bool
         */
        bool is_delimiter(char ch) const
        {
            return ch == ' ' || ch == '\t' || ch == '\n' || ch == Stream::eof;
        }

        /**
         * @brief   匹配字符串
         * @param   pattern     目标字符串
         * @param   end         结尾字符
         * @return  bool
         */
        bool match(std::string pattern, Match_End end);

        /**
         * @brief   匹配字符串+任意字符
         * @param   pattern     目标字符串
         * @param   end         结尾字符
         * @return  bool
         */
        bool match(std::string pattern, std::string end);

        /**
         * @brief   匹配其中一个字符
         * @param   pattern     目标字符集
         * @return  bool
         */
        bool match_any_of(std::string pattern) const
        {
            return pattern.find(input_.peek()) != -1;
        }

        /**
         * @brief   VALUE 进入状态检查
         * @return  bool
         */
        bool match_value()
        {
            if (match(":", Match_End::BLANK))
                return true;

            if (in_block())
                return false;

            return can_be_json ? input_.peek() == ':'
                               : match(":", std::string("]},"));
        }
    };

} // namespace cyaml

#endif // CYAML_SCANNER_H
