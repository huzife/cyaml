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
#include <istream>
#include <stack>
#include <queue>

namespace cyaml
{
    /**
     * @class   Scanner
     * @brief   YAML 词法分析器
     */
    class Scanner
    {
    private:
        std::istream &input_stream_; // 输入流

        Mark mark_ = Mark(1, 1);  // 当前输入位置
        uint32_t tab_cnt_ = 0;    // 该行'\t'个数
        uint32_t cur_indent_ = 0; // 当前 token 的缩进长度
        uint32_t min_indent_ = 0; // 用于限制多行字符串缩进
        bool ignore_tab_ = true;  // 是否忽略 '\t'，用于计算缩进
        bool input_end_ = false;  // 记录是否读取完输入流
        bool scan_end_ = false;   // 记录是否解析完所有 token

        std::stack<Indent> indent_; // 缩进状态栈

        std::queue<Token> token_; // 暂存下一个 token
        std::deque<char> chars_;  // 存放读取的字符

        std::string value_; // 当前读取的字面量

        char replace_ = ' ';       // 字符串换行时替换的字符
        bool append_ = false;      // 字符串末尾是否添加换行
        bool in_special_ = false;  // 是否正在扫描特殊字符串
        bool need_scalar_ = false; // 是否需要读取标量

    public:
        /**
         * @brief   Scanner 类构造函数
         * @param   std::istream    输入流
         * @retval  Scanner 对象
         */
        Scanner(std::istream &in);

        /**
         * @brief   获取下一个 token
         * @details 从输入流扫描并解析出下一个 token
         * @return  Token
         * @retval  下一个 token
         */
        Token next_token();

        /**
         * @brief   查看但不消耗下一个 token
         * @details 该解析器通过基于 YAML 的 LL(1)文法实现，
         *          需要向前查看一个符号而不消耗这个 token
         * @return  Token
         * @retval  下一个 token
         */
        Token lookahead();

        /**
         * @brief   返回当前位置标记
         * @return  Mark
         * @retval  词法分析器当前读取输入的位置结构体
         */
        Mark mark()
        {
            return mark_;
        }

        /**
         * @brief   返回当前字符行号
         * @return  uint32_t
         * @retval  词法分析器当前读取位置的行号
         */
        uint32_t line_no()
        {
            return mark_.line;
        }

        /**
         * @brief   返回当前字符列号
         * @return  uint32_t
         * @retval  词法分析器当前读取位置的列号
         */
        uint32_t col_no()
        {
            return mark_.column;
        }

        /**
         * @brief   获取当前缩进位置
         * @return  uint32_t
         * @retval  当前输入流读取位置对应缩进值
         */
        uint32_t get_cur_indent() const
        {
            return mark_.column - tab_cnt_ - 1;
        }

        /**
         * @brief   返回 Scanner 扫描状态
         * @return  bool
         * @retval  true:   扫描结束，无后续 token
         *          false:  扫描未结束，还有未解析 token
         */
        bool end()
        {
            return scan_end_ && token_.empty();
        }

    private:
        /**
         * @brief   读取下一个字符
         * @return  char
         * @retval  输入流的下一个字符
         */
        char next_char();

        /**
         * @brief   查看下一个字符
         * @return  char
         */
        char next() const
        {
            assert(!chars_.empty());
            return chars_.front();
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
            while (!input_end_ && is_delimiter(next())) {
                next_char();
            }
        }

        /**
         * @brief   跳过注释
         * @return  void
         */
        void skip_comment()
        {
            if (next() == '#') {
                while (!input_end_ && next() != '\n') {
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
         * @brief   检查是否需要添加 null
         * @details 识别到 BLOCK_SEQ_ENTRY 或 KEY 时，检查上一个 token 是否为空
         * @param   Indent_Type     当前缩进类型
         * @return  void
         */
        void fill_null(Indent_Type type);

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
         * @brief   扫描 BLOCK_SEQ_ENTRY
         * @return  void
         */
        void scan_block_seq_entry();

        /**
         * @brief   扫描一个标量
         * @details 由于 YAML 的字符串使用很灵活，词法分析阶段无法区分
         *          string 和其他标量，所以目前先全部按照字符串解析
         * @return  void
         */
        void scan_scalar();

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
         */
        void start_scalar()
        {
            need_scalar_ = true;
            min_indent_ = cur_indent_ + 1;
        }

        /**
         * @brief   扫描标量结束后调用
         */
        void end_scalar()
        {
            need_scalar_ = false;
            min_indent_ = 0;
        }

        /**
         * @brief   处理转义字符
         * @return  char
         * @retval  解析得到的转义字符
         */
        char escape();

        /**
         * @brief   推入缩进值
         * @param   Indent_Type     缩进类型
         */
        void push_indent(Indent_Type type);

        /**
         * @brief   弹出缩进值
         */
        void pop_indent();

        /**
         * @brief   匹配剩余所有缩进
         */
        void pop_all_indent()
        {
            while (!indent_.empty()) {
                token_.push(Token(indent_.top(), false));
                indent_.pop();
            }
        }

        /**
         * @brief   判断字符是否属于分隔符
         * @return  bool
         */
        bool is_delimiter(char ch)
        {
            return ch == ' ' || ch == '\t' || ch == '\n' || ch == -1;
        }

        /**
         * @brief   判断字符是否属于数字
         * @return  bool
         */
        bool is_number(char ch)
        {
            return '0' <= ch && ch <= '9';
        }

        /**
         * @brief   判断字符是否属于字母
         * @return  bool
         */
        bool is_letter(char ch)
        {
            return 'A' <= ch && ch <= 'Z' || 'a' <= ch && ch <= 'z';
        }

        /**
         * @brief   判断字符是否属于字符串起始符
         * @return  bool
         */
        bool is_string_begin(char ch)
        {
            return ch == '>' || ch == '\'' || ch == '\"' || ch == '|' ||
                   is_letter(ch);
        }

        /**
         * @brief   匹配字符串
         * @return  bool
         */
        bool match(std::string pattern, bool end_with_delimiter = false);
    };

} // namespace cyaml

#endif // CYAML_SCANNER_H
