/**
 * @file        indent.h
 * @brief       缩进类头文件
 * @details     存放缩进信息，包括缩进长度、缩进类型
 * @date        2023-7-27
 */

#ifndef CYAML_INDENT_H
#define CYAML_INDENT_H

#include <stdint.h>

namespace cyaml
{
    /**
     * @enum    Flow_Type
     * @brief   用于判断当前流式节点的类型
     */
    enum class Flow_Type
    {
        NONE,
        MAP,
        SEQ
    };

    /**
     * @enum    Indent_Type
     * @brief   缩进类型
     */
    enum class Indent_Type
    {
        NONE,
        MAP,
        SEQ
    };

    /**
     * @struct  Indent
     * @brief   缩进类
     */
    struct Indent
    {
        Indent_Type type;
        uint32_t len;
    };
} // namespace cyaml

#endif // CYAML_INDENT_H
