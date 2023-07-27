/**
 * @file        exceptions.h
 * @brief       异常类型
 * @details     该文件声明了 cyaml 的异常类型，主要用于解析和运行时错误处理
 * @date        2023-7-27
 */

#ifndef CYAML_EXCEPTIONS_H
#define CYAML_EXCEPTIONS_H

#include "error/error_msgs.h"
#include "type/mark.h"
#include <stdexcept>
#include <string>

namespace cyaml
{
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
        Exception(const std::string &msg, const Mark &mark = Mark());
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

} // namespace cyaml

#endif // CYAML_EXCEPTIONS_H
