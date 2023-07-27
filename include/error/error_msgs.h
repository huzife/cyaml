/**
 * @file        error_msgs.h
 * @brief       错误消息
 * @details     该文件定义了 cyaml 运行中可能用到的错误消息
 * @date        2023-7-27
 */

#ifndef CYAML_ERROR_MSGS_H
#define CYAML_ERROR_MSGS_H

namespace cyaml
{
    namespace error_msgs
    {
        const char *const UNKNOWN_TOKEN = "unknown token";
        const char *const UNKNOWN_ESCAPE = "unknown escape";
        const char *const EOF_IN_SCALAR = "illegal EOF in scalar";
    }
} // namespace cyaml

#endif // CYAML_ERROR_MSGS_H
