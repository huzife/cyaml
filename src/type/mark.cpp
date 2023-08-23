/**
 * @file        mark.cpp
 * @brief       读取位置标记源文件
 * @details     用 Mark 结构存放行列位置，方便传参和错误处理
 * @date        2023-7-27
 */

#include "cyaml/type/mark.h"

namespace cyaml
{
    // 将构造函数外联定义
    Mark::Mark(uint32_t line, uint32_t column): line(line), column(column) {}

} // namespace cyaml
