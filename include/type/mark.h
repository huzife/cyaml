/**
 * @file        mark.h
 * @brief       读取位置标记头文件
 * @details     用 Mark 结构存放行列位置，方便传参和错误处理
 * @date        2023-7-27
 */

#ifndef CYAML_MARK_H
#define CYAML_MARK_H

#include <stdint.h>

namespace cyaml
{
    struct Mark
    {
        uint32_t line = 0;
        uint32_t column = 0;

        Mark() = default;
        Mark(const Mark&) = default;
        Mark(uint32_t line, uint32_t column);

        bool is_null() const
        {
            return line == 0 && column == 0;
        }
    };

} // namespace cyaml

#endif // CYAML_MARK_H
