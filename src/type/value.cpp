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
        if (!node_->is_sequence())
            throw Dereference_Exception();

        ///< @todo  out_of_bound exception
        if (index >= node_->sequence_data_.size())
            throw Dereference_Exception();

        return Value(node_->sequence_data_[index]);
    }

    Value Value::operator[](std::string key)
    {
        if (!node_->is_map())
            throw Dereference_Exception();

        ///< @todo  key_not_found exception
        if (!find(key))
            throw Dereference_Exception();

        return Value(node_->map_data_[key]);
    }

    uint32_t Value::size() const
    {
        switch (node_->type_) {
        case Node_Type::MAP:
            return node_->map_data_.size();
        case Node_Type::SEQUENCE:
            return node_->sequence_data_.size();
        case Node_Type::SCALAR:
            return node_->scalar_data_.size();
        }
    }

} // namespace cyaml
