/**
 * @file        node_builder.cpp
 * @brief       节点树构建器源文件
 * @details     包含节点构建事件定义
 * @date        2023-8-15
 */

#include "parser/node_builder.h"
#include "parser/api.h"
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
        auto &node = push();
        node = std::make_shared<Node>(Node_Type::MAP);
        node->set_style(style);

        if (!anchor.empty()) {
            anchor_map_[anchor] = node;
        }

        if (map_depth_ > key_.size()) {
            complex_key_depth_++;
        }
        map_depth_++;
    }

    void Node_Builder::on_map_end()
    {
        assert(map_depth_ > 0);
        if (complex_key_depth_ > 0) {
            complex_key_depth_--;
        }
        map_depth_--;
        pop();
    }

    void Node_Builder::on_seq_start(
            const Mark &mark,
            std::string anchor,
            Node_Style style)
    {
        mark_ = mark;
        auto &node = push();
        node = std::make_shared<Node>(Node_Type::SEQ);
        node->set_style(style);

        if (!anchor.empty()) {
            anchor_map_[anchor] = node;
        }
    }

    void Node_Builder::on_seq_end()
    {
        pop();
    }

    void Node_Builder::on_scalar(
            const Mark &mark,
            std::string anchor,
            std::string value)
    {
        mark_ = mark;
        auto &node = push();
        node = std::make_shared<Node>(value);

        if (!anchor.empty()) {
            anchor_map_[anchor] = node;
        }

        pop();
    }

    void Node_Builder::on_null(const Mark &mark, std::string anchor)
    {
        mark_ = mark;
        auto &node = push();
        node = std::make_shared<Node>();

        if (!anchor.empty()) {
            anchor_map_[anchor] = node;
        }

        pop();
    }

    void Node_Builder::on_alias(const Mark &mark, std::string anchor)
    {
        mark_ = mark;
        auto &node = push();
        auto iter = anchor_map_.find(anchor);
        if (iter == anchor_map_.end()) {
            throw Parse_Exception(error_msgs::UNKNOWN_ANCHOR, mark_);
        }
        *node = *(iter->second);

        pop();
    }

    Node Node_Builder::root()
    {
        return *root_;
    }

    Node_Ptr &Node_Builder::push()
    {
        node_.push(std::make_shared<Node>());
        return node_.top();
    }

    void Node_Builder::pop()
    {
        assert(!node_.empty());

        auto node = node_.top();
        node_.pop();

        // 根节点
        if (node_.empty()) {
            root_ = node;
            return;
        }

        // 其他节点
        auto &top = node_.top();

        if (top->is_map()) {
            uint32_t depth = map_depth_ - complex_key_depth_;
            assert(key_.size() <= depth);
            if (key_.size() == depth) {
                insert(key_.top(), node);
                key_.pop();
            } else {
                key_.push(node);
            }
        } else if (top->is_seq()) {
            top->push_back(*node);
        } else {
            while (!key_.empty()) {
                key_.pop();
            }
            while (!node_.empty()) {
                node_.pop();
            }
            assert(false);
        }
    }

    void Node_Builder::insert(Node_Ptr &key, Node_Ptr &value)
    {
        assert(!node_.empty());
        if (node_.top()->contain(key)) {
            throw Representation_Exception(error_msgs::DUPLICATED_KEY, mark_);
        }

        node_.top()->insert(*key, *value);
    }

} // namespace cyaml
