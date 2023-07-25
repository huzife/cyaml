/**
 * @file        scanner.h
 * @brief       YAML 解析器词法分析部分 Scanner 类头文件
 * @details     主要包含 YAML 的 Scanner 类定义
 * @date        2023-7-20
 */

#ifndef CYAML_SCANNER_H
#define CYAML_SCANNER_H

#include "parser/token.h"
#include <istream>
#include <stack>

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

        uint32_t line_ = 1;       // 当前行号
        uint32_t col_ = 1;        // 当前列号
        uint32_t tab_cnt_ = 0;    // 该行'\t'个数
        uint32_t indent_ = 0;     // 当前 token 的缩进长度
        uint32_t min_indent_ = 0; // 用于限制多行字符串缩进
        bool ignore_tab_ = true;  // 是否忽略 '\t'，用于计算缩进
        bool input_end_ = false;  // 记录是否读取完输入流
        bool scan_end_ = false;   // 记录是否解析完所有 token

        Token next_token_;         // 暂存下一个 token
        char next_char_ = -1;      // 暂存下一个字符
        char next2_char_ = -1;     // 暂存向前的第二个字符
        bool next2_valid_ = false; // 记录 next2_char_ 是否可用

        std::string value_; // 当前读取的字面量

        char replace_ = ' ';  // 字符串换行时替换的字符
        bool append_ = false; // 字符串末尾是否添加换行
        bool normal_ = false; // 保留或折叠符号后处理普通字符串

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
         * @brief   返回当前字符行号
         * @return  uint32_t
         * @retval  词法分析器当前读取位置的行号
         */
        uint32_t line_no()
        {
            return line_;
        }

        /**
         * @brief   返回当前字符列号
         * @return  uint32_t
         * @retval  词法分析器当前读取位置的列号
         */
        uint32_t col_no()
        {
            return col_;
        }

        /**
         * @brief   返回 Scanner 扫描状态
         * @return  bool
         * @retval  true:   扫描结束，无后续 token
         *          false:  扫描未结束，还有未解析 token
         */
        bool end()
        {
            return scan_end_;
        }

    private:
        /**
         * @brief   读取下一个字符
         * @return  char
         * @retval  输入流的下一个字符
         */
        char next_char();

        /**
         * @brief   扫描并解析一个 token
         * @return  void
         */
        void scan();

        /**
         * @brief   更新缩进值
         * @return  void
         */
        void update_indent();

        /**
         * @brief   扫描一个标量
         * @details 由于 YAML 的字符串使用很灵活，词法分析阶段无法区分
         *          string 和其他标量，所以目前先全部按照字符串解析
         * @return  void
         */
        void scan_scalar();

        /**
         * @brief   扫描一个符号，如 {}, [], :, --- 等
         * @return  void
         */
        void scan_operator();

        /**
         * @brief   扫描特殊字符串，包括保留换行和折叠换行
         * @details 该函数实际上只是识别保留换行和折叠换行相关符号，
         *          然后调用 scan_normal_string()
         * @return  void
         */
        void scan_special_string();

        /**
         * @brief   扫描带引号的两种字符串
         * @return  void
         */
        void scan_quote_string();

        /**
         * @brief   扫描字符串
         * @details 结合缩进检测字符串边界
         * @return  void
         */
        void scan_normal_string(
                char replace = ' ',
                bool append_newline = false);

        /**
         * @brief   处理转义字符
         * @return  char
         * @retval  解析得到的转义字符
         */
        char escape();

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
         * @brief   判断字符是否属于操作符
         * @return  bool
         */
        bool is_operator(char ch)
        {
            return ch == '-' || ch == ':' || ch == '{' || ch == '}' ||
                   ch == '[' || ch == ']';
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
    };

} // namespace cyaml

#endif // CYAML_SCANNER_H
