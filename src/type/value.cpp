/**
 * @file        value.cpp
 * @brief       YAML 数据类，可通过 YAML 数据节点访问实际数据
 * @details     主要包含 YAML 的 Value 类实现
 * @date        2023-7-26
 */

#include "type/value.h"
#include "error/exceptions.h"

namespace cyaml
{
    Value::Value(Node_Ptr &node): node_(node) {}

    Value Value::operator[](uint32_t index)
    {
        if (!node_->is_seq())
            throw Dereference_Exception();

        ///< @todo  out_of_range exception
        if (index >= node_->seq_data_.size())
            throw Dereference_Exception();

        return Value(node_->seq_data_[index]);
    }

    Value Value::operator[](const std::string &key)
    {
        if (!node_->is_map())
            throw Dereference_Exception();

        ///< @todo  key_not_found exception
        if (!find(key))
            throw Dereference_Exception();

        return Value(node_->map_data_[key]);
    }

    Value Value::operator[](const Value &key)
    {
        if (!node_->is_map())
            throw Dereference_Exception();

        ///< @todo  key_not_found exception
        if (!find(key))
            throw Dereference_Exception();

        return Value(node_->map_data_[key.as<std::string>()]);
    }

    Value &Value::operator=(const Value &rhs)
    {
        node_ = rhs.node_;

        ///< @todo  处理 yaml 引用

        return *this;
    }

    bool Value::find(std::string key) const
    {
        if (!is_map())
            return false;

        return node_->map_data_.find(key) != node_->map_data_.end();
    }

    bool Value::find(const Value &value) const
    {
        if (!is_map())
            return false;

        std::string key = value.as<std::string>();
        return node_->map_data_.find(key) != node_->map_data_.end();
    }

    uint32_t Value::size() const
    {
        switch (node_->type_) {
        case Node_Type::MAP:
            return node_->map_data_.size();
        case Node_Type::SEQ:
            return node_->seq_data_.size();
        case Node_Type::SCALAR:
            return node_->scalar_data_.size();
        }

        return 0;
    }

} // namespace cyaml
