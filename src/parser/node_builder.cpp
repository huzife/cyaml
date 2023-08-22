/**
 * @file        node_builder.cpp
 * @brief       节点树构建器源文件
 * @details     包含节点构建事件定义
 * @date        2023-8-15
 */

#include "parser/node_builder.h"
#include <assert.h>

namespace cyaml
{
    Node_Builder::Node_Builder() {}

    void Node_Builder::on_document_end()
    {
        anchor_map_.clear();
    }

    void Node_Builder::on_map_start(
            const Mark &mark,
            std::string anchor,
            Node_Style style)
    {
        mark_ = mark;
        auto node = std::make_shared<Node>(Node_Type::MAP);
        nodes_.push(node);
        node->set_style(style);

        if (!anchor.empty()) {
            anchor_map_[anchor] = node;
        }
    }

    void Node_Builder::on_map_end()
    {
        pop_node();
    }

    void Node_Builder::on_seq_start(
            const Mark &mark,
            std::string anchor,
            Node_Style style)
    {
        mark_ = mark;
        auto node = std::make_shared<Node>(Node_Type::SEQ);
        nodes_.push(node);
        node->set_style(style);

        if (!anchor.empty()) {
            anchor_map_[anchor] = node;
        }
    }

    void Node_Builder::on_seq_end()
    {
        pop_node();
    }

    void Node_Builder::on_scalar(
            const Mark &mark,
            std::string anchor,
            std::string value)
    {
        mark_ = mark;
        auto node = std::make_shared<Node>(value);
        nodes_.push(node);

        if (!anchor.empty()) {
            anchor_map_[anchor] = node;
        }

        pop_node();
    }

    void Node_Builder::on_null(const Mark &mark, std::string anchor)
    {
        mark_ = mark;
        auto node = std::make_shared<Node>();
        nodes_.push(node);

        if (!anchor.empty()) {
            anchor_map_[anchor] = node;
        }

        pop_node();
    }

    void Node_Builder::on_alias(const Mark &mark, std::string anchor)
    {
        mark_ = mark;
        auto node = std::make_shared<Node>();
        nodes_.push(node);
        auto iter = anchor_map_.find(anchor);
        if (iter == anchor_map_.end()) {
            throw Parse_Exception(error_msgs::UNKNOWN_ANCHOR, mark_);
        }
        *node = *(iter->second);

        pop_node();
    }

    Node Node_Builder::root()
    {
        return *root_;
    }

    void Node_Builder::pop_node()
    {
        assert(!nodes_.empty());

        auto node = nodes_.top();
        nodes_.pop();

        // 根节点
        if (nodes_.empty()) {
            root_ = node;
            return;
        }

        // 其他节点
        auto top = nodes_.top();

        if (keys_.find(top) != keys_.end()) {
            nodes_.pop();
            keys_.erase(top);
            insert(top, node);
        } else if (top->is_map()) {
            keys_.insert(node);
            nodes_.push(node);
        } else if (top->is_seq()) {
            top->push_back(*node);
        } else {
            assert(false);
        }
    }

    void Node_Builder::insert(Node_Ptr &key, Node_Ptr &value)
    {
        assert(!nodes_.empty());
        if (nodes_.top()->contain(key)) {
            throw Representation_Exception(error_msgs::DUPLICATED_KEY, mark_);
        }

        nodes_.top()->insert(*key, *value);
    }

} // namespace cyaml
