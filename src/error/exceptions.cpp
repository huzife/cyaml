/**
 * @file        exceptions.cpp
 * @brief       异常类型实现
 * @details     该文件实现了 cyaml 的异常类型，主要用于解析和运行时错误处理
 * @date        2023-7-27
 */

#include "error/exceptions.h"

namespace cyaml
{
    std::string unexpected_token_msg(Token wrong_token)
    {
        return "unexpected token '" +
               token_type_to_string(wrong_token.token_type()) + '\'';
    }

    std::string unexpected_token_msg(
            Token_Type expected_type,
            Token wrong_token)
    {
        std::string ret = "expected a '" +
                          token_type_to_string(expected_type) +
                          "', but received a '" +
                          token_type_to_string(wrong_token.token_type()) + "'";

        if (wrong_token.token_type() == Token_Type::SCALAR) {
            ret += ", value = \"" + wrong_token.value() + '\"';
        }

        return ret;
    }

    const std::string Exception::build_what(
            const std::string &msg,
            const Mark &mark)
    {
        if (mark.is_null())
            return msg;

        return "cyaml error at line " + std::to_string(mark.line) +
               ", column " + std::to_string(mark.column) + ": " + msg;
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

    Dereference_Exception::Dereference_Exception()
        : Representation_Exception(error_msgs::BAD_DEREFERENCE, Mark())
    {
    }

    Convertion_Exception::Convertion_Exception()
        : Representation_Exception(error_msgs::BAD_CONVERTION, Mark())
    {
    }

    Exception::~Exception() = default;
    Parse_Exception::~Parse_Exception() = default;
    Representation_Exception::~Representation_Exception() = default;
    Dereference_Exception::~Dereference_Exception() = default;
    Convertion_Exception::~Convertion_Exception() = default;

} // namespace cyaml
