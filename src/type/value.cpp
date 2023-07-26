/**
 * @file        value.cpp
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Value 类实现
 * @date        2023-7-26
 */

#include "type/value.h"

namespace cyaml
{
    Value::Value(Node_Ptr &node): node_(node) {}

} // namespace cyaml
