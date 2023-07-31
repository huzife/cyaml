/**
 * @file        value.h
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Value 类声明
 * @date        2023-7-25
 */

#ifndef CYAML_VALUE_H
#define CYAML_VALUE_H

#include "type/node.h"
#include "type/convert.h"
#include "error/exceptions.h"
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
        T as()
        {
            T ret;
            if (Convert<T>::decode(*node_, ret))
                return ret;

            throw Convertion_Exception();
        }

        /**
         * @brief   获取值的类型
         * @return  Node_Type
         * @retval  该 Value 对应节点数据类型枚举值
         */
        Node_Type type() const
        {
            return node_->type_;
        }

        /**
         * @brief   判断是否为 map
         * @return bool
         */
        bool is_map() const
        {
            return node_->is_map();
        }

        /**
         * @brief   判断是否为 sequence
         * @return bool
         */
        bool is_sequence() const
        {
            return node_->is_sequence();
        }

        /**
         * @brief   判断是否为 scalar
         * @return bool
         */
        bool is_scalar() const
        {
            return node_->is_scalar();
        }

        /**
         * @brief   判断是否为空值
         * @return  bool
         */
        bool is_null() const
        {
            return node_->is_null();
        }

        /**
         * @brief   获取数据长度
         * @return  uint32_t
         * @retval  自动判断数据类型，返回相应长度
         */
        uint32_t size() const;

        /**
         * @brief   查找 key
         * @param   std::string     key
         * @return  bool
         * @retval  该键是否存在
         */
        bool find(std::string key) const
        {
            return (node_->is_map() &&
                    node_->map_data_.find(key) != node_->map_data_.end());
        }

#ifdef CYAML_DEBUG
        // 临时测试用
        std::string scalar_value()
        {
            return node_->scalar_data_;
        }
#endif
    };
} // namespace cyaml

#endif // CYAML_VALUE_H
