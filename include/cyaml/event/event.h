/**
 * @file        event.h
 * @brief       SAX 解析接口
 * @details     主要包含解析过程触发的一组事件
 * @date        2023-8-15
 */

#ifndef CYAML_EVENT_H
#define CYAML_EVENT_H

#include "cyaml/type/node/node.h"
#include <string>

namespace cyaml
{
    class Event_Handler
    {
    public:
        Event_Handler() = default;
        virtual ~Event_Handler() = default;

        virtual void on_document_start(const Mark &mark) = 0;

        virtual void on_document_end() = 0;

        virtual void on_map_start(
                const Mark &mark,
                std::string anchor,
                Node_Style style) = 0;

        virtual void on_map_end() = 0;

        virtual void on_seq_start(
                const Mark &mark,
                std::string anchor,
                Node_Style style) = 0;

        virtual void on_seq_end() = 0;

        virtual void
        on_scalar(const Mark &mark, std::string anchor, std::string value) = 0;

        virtual void on_null(const Mark &mark, std::string anchor) = 0;

        virtual void on_anchor(const Mark &mark, std::string anchor) = 0;

        virtual void on_alias(const Mark &mark, std::string anchor) = 0;
    };

} // namespace cyaml

#endif // CYAML_EVENT_H
