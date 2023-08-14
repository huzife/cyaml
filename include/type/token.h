/**
 * @file        token.h
 * @brief       YAML 解析器 Token 类头文件
 * @details     主要包含 YAML 的 Token 类声明
 * @date        2023-7-20
 */

#ifndef CYAML_TOKEN_H
#define CYAML_TOKEN_H

#include "type/indent.h"
#include "type/mark.h"
#include <string>
#include <ostream>
#include <assert.h>

namespace cyaml
{
    /**
     * @enum    Token_Type
     * @brief   声明 Token 类型
     */
    enum class Token_Type
    {
        NONE,
        STREAM_START,    // 输入流开始
        STREAM_END,      // 输入流结束
        DOC_START,       // ---
        DOC_END,         // ...
        BLOCK_ENTRY,     // -
        BLOCK_MAP_START, // 进入 block_map
        BLOCK_MAP_END,   // 退出 block_map
        BLOCK_SEQ_START, // 进入 block_seq
        BLOCK_SEQ_END,   // 退出 block_seq
        FLOW_ENTRY,      // ,
        FLOW_MAP_START,  // {
        FLOW_MAP_END,    // }
        FLOW_SEQ_START,  // [
        FLOW_SEQ_END,    // ]
        KEY,             // key 标志
        VALUE,           // value 标志
        SCALAR,          // 标量
        ANCHOR,          // &
        ALIAS,           // *
    };

    /**
     * @enum    Collection_Flag
     * @brief   集合开始或结束标志
     */
    enum class Collection_Flag
    {
        START,
        END
    };

    /**
     * @class   Token
     * @brief   存储 token 字面量、类型信息
     */
    class Token
    {
    private:
        Token_Type token_type_;  // token 类型
        std::string value_;      // 字面量
        Mark mark_;              // 位置

    public:
        /**
         * @brief   Token 类默认构造函数
         * @retval  Token 对象
         */
        Token();

        /**
         * @brief   Token 类构造函数
         * @param   Token_Type      token 类型
         * @param   Mark            token 位置
         * @retval  Token 对象
         */
        Token(Token_Type type, Mark mark);

        /**
         * @brief   Token 类构造函数
         * @param   Token_Type      token 类型
         * @param   std::string     token 字面量
         * @param   Mark            token 位置
         * @retval  Token 对象
         */
        Token(Token_Type type, std::string value, Mark mark);

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
         * @brief   判断 token 是否拥有值
         * @details 目前只有 KEY 和 SCALAR 有值
         * @return  bool
         */
        bool is_scalar() const
        {
            return token_type_ == Token_Type::SCALAR;
        }

        /**
         * @brief   获取 token 的标量值
         * @return  std::string
         * @retval  token 的标量值
         */
        std::string value() const
        {
            assert(is_scalar());
            return value_;
        }

        /**
         * @brief   获取 token 位置
         * @return  Mark
         * @retval  token 在输入流中的位置
         */
        Mark mark() const
        {
            return mark_;
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
    std::string token_type_to_string(Token_Type type);

    /**
     * @brief   缩进枚举类型转换为 Token 枚举类型
     * @param   Indent_Type     Indnet 枚举类型
     * @param   bool            是否是一个集合的开始
     * @return  Token_Type
     */
    Token_Type from_indent_type(Indent_Type type, Collection_Flag flag);

    /**
     * @brief   流枚举类型转换为 Token 枚举类型
     * @param   Flow_Type       Indnet 枚举类型
     * @param   bool            是否是一个集合的开始
     * @return  Token_Type
     */
    Token_Type from_flow_type(Flow_Type type, Collection_Flag flag);

} // namespace cyaml

#endif // CYAML_TOKEN_H
