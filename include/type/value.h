/**
 * @file        value.h
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Value 类定义
 * @date        2023-7-25
 */

#ifndef CYAML_VALUE_H
#define CYAML_VALUE_H

#include "type/node.h"
#include <memory>

namespace cyaml
{
    /**
     * @class   Value
     * @brief   YAML 数据类
     */
    class Value
    {
    private:
        Node::Ptr node_;
    };
} // namespace cyaml

#endif // CYAML_VALUE_H
