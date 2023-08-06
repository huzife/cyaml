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
        Value() = default;
        Value(const Value &) = default;

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
         * @brief   Value 绑定引用
         * @param   const Value &
         * @return  Value &
         */
        Value &operator=(const Value &rhs);

        /**
         * @brief   Value 赋值
         * @tparam  T           值类型
         * @param   const T &   常量左值引用
         * @return  void
         */
        template<typename T>
        void operator=(const T &rhs)
        {
            ///< @todo  检查原节点，清除状态

            *node_ = Convert<T>::encode(rhs);
        }

        /**
         * @brief   获取标量值
         * @tparam  T   转换类型
         * @return  T
         * @retval  转换后的标量值
         */
        template<typename T>
        T as() const
        {
            T ret;
            if (Convert<T>::decode(*node_, ret))
                return ret;

            throw Convertion_Exception();
        }

        /**
         * @brief   根据字符串查找 key
         * @param   std::string     key
         * @return  bool
         * @retval  该键是否存在
         */
        bool find(std::string key) const;

        /**
         * @brief   根据 Value 查找 key
         * @param   const Value &   value
         * @return  bool
         * @retval  该键是否存在
         */
        bool find(const Value &value) const;

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
        bool is_seq() const
        {
            return node_->is_seq();
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
    };
} // namespace cyaml

#endif // CYAML_VALUE_H
