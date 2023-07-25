/**
 * @file        token.h
 * @brief       YAML 解析器 Token 类头文件
 * @details     主要包含 YAML 的 Token 类定义
 * @date        2023-7-20
 */

#ifndef CYAML_TOKEN_H
#define CYAML_TOKEN_H

#include <string>
#include <ostream>
#include <assert.h>

namespace cyaml
{
    /**
     * @enum    Token_Type
     * @brief   定义了 Token 类型
     */
    enum class Token_Type
    {
        NONE,
        START,    // ---
        END,      // ...
        DASH,     // -
        COLON,    // :
        LBRACE,   // {
        RBRACE,   // }
        LBRACKET, // [
        RBRACKET, // ]
        PRESERVE, // |
        FOLD,     // >
        SQUOTE,   // '
        DQUOTE,   // "
        KEY,      // 对象 key
        SCALAR    // 标量
    };

    /**
     * @enum    String_Type
     * @brief   定义了字符串类型
     */
    enum class String_Type
    {
        NORMAL_STRING, // 不带引号
        SQUOTE_STRING, // 带单引号
        DQUOTE_STRING  // 带双引号
    };

    /**
     * @class   Token
     * @brief   存储 token 字面量、类型信息
     */
    class Token
    {
    private:
        Token_Type token_type_;   // token 类型
        String_Type string_type_; // 字符串类型
        std::string value_;       // 字面量
        uint32_t indent_;

    public:
        /**
         * @brief   Token 类默认构造函数
         * @retval  Token 对象
         */
        Token();

        /**
         * @brief   Token 类构造函数
         * @param   Token_Type      token 类型
         * @param   std::string     token 字面量
         * @retval  Token 对象
         */
        Token(Token_Type type, std::string value, uint32_t indent);

        /**
         * @brief   Token 类构造函数
         * @param   String_Type     字符串类型
         * @param   std::string     token 字面量
         * @retval  Token 对象
         */
        Token(String_Type str_type, std::string value, uint32_t indent);

        /**
         * @brief   获取 token 类型
         * @return  Token_Type
         * @retval  该 token 的类型
         */
        Token_Type token_type() const
        {
            return token_type_;
        }

        /**
         * @brief   获取 token 的字符串类型
         * @return  String_Type
         * @retval  若 token 表示字符串标量，返回值为字符串类型
         *          若 token 为其他标量，返回值无意义
         */
        String_Type string_type() const
        {
            assert(token_type_ == Token_Type::SCALAR);
            return string_type_;
        }

        /**
         * @brief   获取 token 的 int 值
         * @return  int
         * @retval  token 所表示的 int 标量值
         */
        int int_value() const
        {
            assert(token_type_ == Token_Type::SCALAR);
            return std::strtoll(value_.c_str(), nullptr, 10);
        }

        /**
         * @brief   获取 token 的 real 值
         * @return  float
         * @retval  token 所表示的 real 标量值
         */
        float real_value() const
        {
            assert(token_type_ == Token_Type::SCALAR);
            return std::strtof(value_.c_str(), nullptr);
        }

        /**
         * @brief   获取 token 的 string 值
         * @return  std::string
         * @retval  token 所表示的 string 标量值
         */
        std::string string_value() const
        {
            assert(token_type_ == Token_Type::SCALAR);
            return value_;
        }

        /**
         * @brief   获取 token 的缩进长度
         * @return  uint32_t
         * @retval  token 的缩进长度(不是行的缩进)
         */
        uint32_t indent() const
        {
            assert(token_type_ != Token_Type::NONE);
            return indent_;
        }

        /**
         * @brief   将 token 转化为 string
         * @return  std::string
         * @retval  (token_type, value)
         */
        std::string to_string() const;

        /**
         * @brief   重载 Token 类<<运算符
         */
        friend std::ostream &operator<<(std::ostream &out, const Token &token);
    };

    /**
     * @brief   Token 枚举类型转换为字符串
     * @param   Token_Type  token类型
     * @return  std::string
     * @retval  Token 类型名
     */
    static std::string token_type_to_string(Token_Type type);

} // namespace cyaml

#endif // CYAML_TOKEN_H
