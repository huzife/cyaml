/**
 * @file        node.h
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Node 类声明
 * @date        2023-7-25
 */

#ifndef CYAML_VALUE_H
#define CYAML_VALUE_H

#include "type/node/node_data.h"
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
     * @enum    Node_Style
     * @brief   节点样式
     */
    enum class Node_Style
    {
        BLOCK,
        FLOW
    };

    /**
     * @class   Node
     * @brief   YAML 数据类
     */
    class Node: public std::enable_shared_from_this<Node>
    {
    private:
        Node_Type type_;     // 节点类型
        Node_Data_Ptr data_; // 节点数据

        Node_Style style_ = Node_Style::BLOCK; // 节点样式

    public:
        Node(Node_Type type = Node_Type::NULL_NODE);
        Node(const Node &node);
        Node(const Node_Ptr &node);
        Node(const std::string &scalar);
        ~Node();

        friend bool operator==(const Node &n1, const Node &n2);
        friend bool operator==(const Node_Ptr &n1, const Node_Ptr &n2);
        friend bool operator!=(const Node &n1, const Node &n2);
        friend bool operator!=(const Node_Ptr &n1, const Node_Ptr &n2);

        /**
         * @brief   获取值的类型
         * @return  Node_Type
         */
        Node_Type type() const
        {
            return type_;
        }

        /**
         * @brief   获取节点样式
         * @return  Node_Style
         */
        Node_Style style() const
        {
            return style_;
        }

        /**
         * @brief   设置节点样式
         * @param   style   节点样式
         * @return  void
         */
        void set_style(Node_Style style)
        {
            style_ = style;
        }

        /**
         * @brief   获取数据长度
         * @return  uint32_t
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
         * @return  bool
         */
        bool is_map() const
        {
            return type_ == Node_Type::MAP;
        }

        /**
         * @brief   判断是否为 sequence
         * @return  bool
         */
        bool is_seq() const
        {
            return type_ == Node_Type::SEQ;
        }

        /**
         * @brief   判断是否为集合
         * @return  bool
         */
        bool is_collection() const
        {
            return is_map() || is_seq();
        }

        /**
         * @brief   判断是否为 scalar
         * @return  bool
         */
        bool is_scalar() const
        {
            return type_ == Node_Type::SCALAR;
        }

        /**
         * @brief   获取标量
         * @return  std::string
         */
        std::string scalar() const
        {
            assert(data_);
            return data_->scalar;
        }

        /**
         * @brief   获取所有 key
         * @return  std::vector<Node>
         */
        std::vector<Node> keys() const;

        /**
         * @brief   获取序列(数组)元素
         * @param   index   元素索引值
         * @return  Node &
         */
        Node &operator[](uint32_t index);
        const Node &operator[](uint32_t index) const;

        /**
         * @brief   查找映射节点
         * @param   key     字符串键
         * @return  Node &
         */
        Node &operator[](const std::string &key);
        const Node &operator[](const std::string &key) const;

        /**
         * @brief   查找映射节点
         * @param   key     节点键
         * @return  Node &
         */
        Node &operator[](const Node &key);
        const Node &operator[](const Node &key) const;

        /**
         * @brief   Node 赋值并绑定绑定引用
         * @param   rhs     目标值
         * @return  Node &
         */
        Node &operator=(const Node &rhs);

        /**
         * @brief   Node 赋值
         * @tparam  T       值类型
         * @param   rhs     目标值
         * @return  Node &
         */
        template<typename T>
        Node &operator=(const T &rhs);

        /**
         * @brief   获取标量值
         * @tparam  T   转换类型
         * @return  T
         */
        template<typename T>
        T as() const;

        /**
         * @brief   根据字符串查找 key
         * @param   key     键
         * @return  bool
         * @retval  true:   键存在
         * @retval  false:  键不存在
         */
        bool contain(std::string key) const;

        /**
         * @brief   根据 Node 查找 key
         * @param   key     键
         * @return  bool
         * @retval  true:   键存在
         * @retval  false:  键不存在
         */
        bool contain(const Node &key) const;

        /**
         * @brief   插入键值对
         * @param   key     键节点
         * @param   value   值节点
         * @return  bool
         * @retval  true:   插入成功
         * @retval  false:  节点无法转换为 map，插入失败
         */
        bool insert(const Node &key, const Node &value);

        /**
         * @brief   数组添加节点
         * @param   node    添加的节点
         * @return  bool
         * @retval  true:   添加成功
         * @retval  false:  节点无法转换为 seq，添加失败
         */
        bool push_back(const Node &node);

        /**
         * @brief   数组添加节点
         * @tparam  T       值类型
         * @param   rhs     值
         * @return  bool
         * @retval  true:   添加成功
         * @retval  false:  节点无法转换为 seq，添加失败
         */
        template<typename T>
        bool push_back(const T &rhs);

        /**
         * @brief   删除元素
         * @param   key     键
         * @return  bool
         * @retval  true:   删除成功
         * @retval  false:  节点不是 map，或此键不存在
         */
        bool erase(const Node &key);

        /**
         * @brief   克隆节点
         * @details 用于赋值时传递节点的值而非引用
         * @return  Node
         */
        Node clone() const
        {
            Node_Ptr node;
            clone(node);
            return *node;
        }

        /**
         * @brief   清空节点
         * @return  void
         */
        void clear()
        {
            reset(type_);
        }

    private:
        /**
         * @brief   重置节点
         * @param   type    重置节点类型
         * @return  void
         */
        void reset(Node_Type type = Node_Type::NULL_NODE)
        {
            type_ = type;
            if (!is_null()) {
                data_->remove_ref(this);
                data_ = std::make_shared<Node_Data>();
            }
        }

        /**
         * @brief   克隆节点内部实现
         * @param   node    节点指针
         * @return  void
         */
        void clone(Node_Ptr &node) const;

        /**
         * @brief   查找 map
         * @param   key     键
         * @return  Map::iterator
         * @retval  map.end():  查找失败
         */
        Map::iterator find(const Node_Ptr &key) const;

        /**
         * @brief   插入键值对
         * @param   key     键节点指针
         * @param   value   值节点指针
         * @return  void
         */
        void insert(const Node_Ptr &key, const Node_Ptr &value)
        {
            assert(!contain(key));
            data_->map.emplace_back(key, value);
        }

        /**
         * @brief   重置节点指向数据
         * @param   data    节点数据指针
         * @return  void
         */
        void assign(Node_Data_Ptr data);
    };
} // namespace cyaml

#endif // CYAML_VALUE_H
