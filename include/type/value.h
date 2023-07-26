/**
 * @file        value.h
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Value 类声明
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
        Node_Ptr node_;

    public:
        /**
         * @brief   Value 构造
         * @param   Node_Ptr    数据节点指针
         * @retval  YAML 数据类对象
         */
        Value(Node_Ptr &node);

        /**
         * @brief   获取序列(数组)元素
         * @param   uint32_t    元素索引值
         * @retval  由传入索引对应元素封装的 Value
         */
        Value operator[](uint32_t index);

        /**
         * @brief   获取映射(对象)元素
         * @param   std::string     键
         * @retval  由传入键对应元素封装的 Value
         */
        Value operator[](std::string key);

        /**
         * @brief   获取标量值
         * @tparam  T   转换类型
         * @return  T
         * @retval  转换后的标量值
         */
        template<typename T>
        T as();

        /**
         * @brief   获取值的类型
         * @return  Data_Type
         * @retval  该 Value 对应节点数据类型枚举值
         */
        Data_Type type()
        {
            return node_->type_;
        }
    };
} // namespace cyaml

#endif // CYAML_VALUE_H
