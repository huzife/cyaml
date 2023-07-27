/**
 * @file        exceptions.cpp
 * @brief       异常类型实现
 * @details     该文件实现了 cyaml 的异常类型，主要用于解析和运行时错误处理
 * @date        2023-7-27
 */

#include "error/exceptions.h"

namespace cyaml
{
    const std::string Exception::build_what(
            const std::string &msg,
            const Mark &mark)
    {
        if (mark.is_null())
            return msg;

        return "cyaml error at line " + std::to_string(mark.line) +
               ", column" + std::to_string(mark.column) + ": " + msg;
    }

    Exception::Exception(const std::string &msg, const Mark &mark)
        : std::runtime_error(build_what(msg, mark)),
          msg_(msg),
          mark_(mark)
    {
    }

    Parse_Exception::Parse_Exception(const std::string &msg, const Mark &mark)
        : Exception(msg, mark)
    {
    }

    Representation_Exception::Representation_Exception(
            const std::string &msg,
            const Mark &mark)
        : Exception(msg, mark)
    {
    }

    Exception::~Exception() = default;
    Parse_Exception::~Parse_Exception() = default;
    Representation_Exception::~Representation_Exception() = default;

} // namespace cyaml
