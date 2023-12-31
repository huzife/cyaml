/**
 * @file        exceptions.h
 * @brief       异常类型
 * @details     该文件声明了 cyaml 的异常类型，主要用于解析和运行时错误处理
 * @date        2023-7-27
 */

#ifndef CYAML_EXCEPTIONS_H
#define CYAML_EXCEPTIONS_H

#include "cyaml/error/error_msgs.h"
#include "cyaml/type/token.h"
#include "cyaml/type/mark.h"
#include <stdexcept>
#include <string>

namespace cyaml
{
    /**
     * @brief   生成 UNEXPECTED TOKEN 错误信息
     * @param   wrong_token     错误 token
     * @return  std::string
     */
    std::string unexpected_token_msg(Token wrong_token);

    /**
     * @brief   生成 UNEXPECTED TOKEN 错误信息
     * @param   expected_type   期望的 token 类型
     * @param   wrong_token     错误 token
     * @return  std::string
     */
    std::string unexpected_token_msg(
            Token_Type expected_type,
            Token wrong_token);

    /**
     * @class   Exception
     * @brief   cyaml 异常类
     * @extends std::runtime_error
     */
    class Exception: public std::runtime_error
    {
    public:
        std::string msg_;
        Mark mark_;

    public:
        Exception(const std::string &msg, const Mark &mark);
        ~Exception() noexcept override;
        Exception(const Exception &) = default;

    private:
        static const std::string build_what(
                const std::string &msg,
                const Mark &mark);
    };

    /**
     * @class   Parse_Exception
     * @brief   解析异常
     * @extends cyaml::Exception
     */
    class Parse_Exception: public Exception
    {
    public:
        Parse_Exception(const std::string &msg, const Mark &mark);
        ~Parse_Exception() noexcept override;
        Parse_Exception(const Parse_Exception &) = default;
    };

    /**
     * @class   Representation_Exception
     * @brief   解析异常
     * @extends cyaml::Exception
     */
    class Representation_Exception: public Exception
    {
    public:
        Representation_Exception(const std::string &msg, const Mark &mark);
        ~Representation_Exception() noexcept override;
        Representation_Exception(const Representation_Exception &) = default;
    };

    /**
     * @class   Dereference_Exception
     * @brief   解引用异常
     * @extends cyaml::Exception
     */
    class Dereference_Exception: public Representation_Exception
    {
    public:
        Dereference_Exception();
        ~Dereference_Exception() noexcept override;
        Dereference_Exception(const Dereference_Exception &) = default;
    };

    /**
     * @class   Convertion_Exception
     * @brief   转换异常
     * @extends cyaml::Exception
     */
    class Convertion_Exception: public Representation_Exception
    {
    public:
        Convertion_Exception();
        ~Convertion_Exception() noexcept override;
        Convertion_Exception(const Convertion_Exception &) = default;
    };

} // namespace cyaml

#endif // CYAML_EXCEPTIONS_H
