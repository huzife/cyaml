/**
 * @file        node_builder.h
 * @brief       用于构建节点树
 * @details     继承 Event_Handler，在事件中构建节点
 * @date        2023-8-15
 */

#ifndef CYAML_NODE_BUILDER_H
#define CYAML_NODE_BUILDER_H

#include "event/event.h"
#include "type/node/node.h"
#include <stack>

namespace cyaml
{
    class Node_Builder: public Event_Handler
    {
    private:
        Node_Ptr root_;
        std::stack<Node_Ptr> nodes_;
        std::unordered_set<Node_Ptr> keys_;
        std::unordered_map<std::string, Node_Ptr> anchor_map_;
        Mark mark_;

    public:
        Node_Builder();
        ~Node_Builder() = default;

        // events derived from Event_Handler
        void on_document_start(const Mark &mark) override
        {
            mark_ = mark;
        };

        void on_document_end() override;

        void on_map_start(
                const Mark &mark,
                std::string anchor,
                Node_Style style) override;

        void on_map_end() override;

        void on_seq_start(
                const Mark &mark,
                std::string anchor,
                Node_Style style) override;

        void on_seq_end() override;

        void on_scalar(const Mark &mark, std::string anchor, std::string value)
                override;

        void on_null(const Mark &mark, std::string anchor) override;

        void on_anchor(const Mark &mark, std::string anchor) override{};

        void on_alias(const Mark &mark, std::string anchor) override;

        /**
         * @brief   返回根节点
         * @return  Node
         */
        Node root();

    private:
        /**
         * @brief   弹出一个节点，建立节点关系
         * @return  void
         */
        void pop_node();

        /**
         * @brief   插入键值对
         * @param   key     键节点
         * @param   value   值节点
         * @return  void
         */
        void insert(Node_Ptr &key, Node_Ptr &value);
    };
} // namespace cyaml

#endif // CYAML_NODE_BUILDER_H
