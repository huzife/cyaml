/**
 * @file        node.h
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Node 类声明
 * @date        2023-7-25
 */

#ifndef CYAML_VALUE_H
#define CYAML_VALUE_H

#include "type/node_data.h"
#include "error/exceptions.h"

namespace cyaml
{
    /**
     * @enum    Node_Type
     * @brief   声明节点的数据类型
     */
    enum class Node_Type
    {
        NULL_NODE,
        MAP,
        SEQ,
        SCALAR
    };

    /**
     * @class   Node
     * @brief   YAML 数据类
     */
    class Node
    {
    private:
        Node_Type type_;     // 节点类型
        Node_Data_Ptr data_; // 节点数据

    public:
        Node();
        Node(Node_Type type);
        Node(const Node_Ptr &node);
        Node(Node_Type type, const Node_Data_Ptr &data);
        Node(const std::string &scalar);
        Node(const Node &) = default;

        /**
         * @brief   获取值的类型
         * @return  Node_Type
         * @retval  该 Node 对应节点数据类型枚举值
         */
        Node_Type type() const
        {
            return type_;
        }

        /**
         * @brief   获取数据长度
         * @return  uint32_t
         * @retval  自动判断数据类型，返回相应长度
         */
        uint32_t size() const;

        /**
         * @brief   判断是否为空值
         * @return  bool
         */
        bool is_null() const
        {
            return type_ == Node_Type::NULL_NODE;
        }

        /**
         * @brief   判断是否为 map
         * @return bool
         */
        bool is_map() const
        {
            return type_ == Node_Type::MAP;
        }

        /**
         * @brief   判断是否为 sequence
         * @return bool
         */
        bool is_seq() const
        {
            return type_ == Node_Type::SEQ;
        }

        /**
         * @brief   判断是否为 scalar
         * @return bool
         */
        bool is_scalar() const
        {
            return type_ == Node_Type::SCALAR;
        }

        /**
         * @brief   获取 map
         */
        type::Map map() const
        {
            assert(data_);
            return data_->map();
        }

        /**
         * @brief   获取 sequence
         */
        type::Sequence seq() const
        {
            assert(data_);
            return data_->seq();
        }

        /**
         * @brief   获取标量
         */
        std::string scalar() const
        {
            assert(data_);
            return data_->scalar();
        }

        /**
         * @brief   获取 keys
         */
        type::Node_List keys() const
        {
            assert(data_);
            return data_->keys();
        }

        /**
         * @brief   获取序列(数组)元素
         * @param   uint32_t    元素索引值
         * @retval  由传入索引对应元素封装的 Node
         */
        Node operator[](uint32_t index);

        /**
         * @brief   查找映射值
         * @param   std::string     键
         * @return  Node
         * @retval  对应元素的 Node
         */
        Node operator[](const std::string &key);

        /**
         * @brief   查找映射值
         * @param   const Node &   键
         * @return  Node
         * @retval  对应元素的 Node
         */
        Node operator[](const Node &key);

        /**
         * @brief   Node 绑定引用
         * @param   const Node &
         * @return  Node &
         */
        Node &operator=(const Node &rhs);

        /**
         * @brief   Node 赋值
         * @tparam  T           值类型
         * @param   const T &   常量左值引用
         * @return  void
         */
        template<typename T>
        Node &operator=(const T &rhs);

        /**
         * @brief   获取标量值
         * @tparam  T   转换类型
         * @return  T
         * @retval  转换后的标量值
         */
        template<typename T>
        T as() const;

        /**
         * @brief   根据字符串查找 key
         * @param   std::string     key
         * @return  bool
         * @retval  该键是否存在
         */
        bool find(std::string key) const;

        /**
         * @brief   根据 Node 查找 key
         * @param   const Node &   value
         * @return  bool
         * @retval  该键是否存在
         */
        bool find(const Node &value) const;

        /**
         * @brief   插入键值对
         * @param   const Node &    键
         * @param   const Node &    值
         * @return  bool
         * @retbal  是否插入成功
         */
        bool insert(const Node &key, const Node &value);

        /**
         * @brief   添加数组元素
         * @param   const Node &
         * @return  bool
         * @retbal  是否添加成功
         */
        bool push_back(const Node &node);
    };
} // namespace cyaml

#endif // CYAML_VALUE_H
