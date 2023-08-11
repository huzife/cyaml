/**
 * @file        serializer.h
 * @brief       YAML 序列化部分 Serializer 类头文件
 * @details     主要包含 YAML 的 Serializer 类声明
 * @date        2023-8-9
 */

#ifndef CYAML_SERIALIZER_H
#define CYAML_SERIALIZER_H

#include "type/node/node.h"
#include "type/mark.h"
#include <ostream>
#include <string>

namespace cyaml
{
    /**
     * @class   Serializer
     * @brief   用于序列化和输出 Node
     */
    class Serializer
    {
    private:
        std::ostream &output_stream_; // 输出流
        uint32_t indent_inc_ = 2;     // 每级缩进长度
        Mark mark_{1, 1};             // 当前输出位置

    public:
        Serializer(std::ostream &out);

        /**
         * @brief   输出 node
         */
        Serializer &operator<<(const Node &node);

        /**
         * @brief   获取当前行
         */
        uint32_t line() const
        {
            return mark_.line;
        }

        /**
         * @brief   获取当前列
         */
        uint32_t column() const
        {
            return mark_.column;
        }

    private:
        /**
         * @brief   计算下一级缩进
         * @param   uint32_t        当前缩进
         * @return  uint32_t
         */
        uint32_t increase(uint32_t indent) const
        {
            return indent + indent_inc_;
        }

        /**
         * @brief   填充空白字符到目标位置
         * @param   uint32_t    缩进位置
         * @return  void
         */
        void fill_blank(uint32_t indent);

        /**
         * @brief   判断节点是否只需要输出一行
         * @param   const Node &    节点
         * @return  bool
         */
        bool line_style(const Node &node) const
        {
            return !node.is_collection() || node.style() == Node_Style::FLOW;
        }

        /**
         * @brief   写入字符串
         * @param   std::string     写入内容
         * @return  void
         */
        void write(std::string str);

        /**
         * @brief   写入指定个数空格
         * @param   int     个数
         * @return  void
         */
        void write_space(int count = 1);

        /**
         * @brief   写入指定个数换行
         * @param   int     个数
         * @return  void
         */
        void write_new_line(int count = 1);

        void write_node(const Node &node, uint32_t indent);
        void write_block_node(const Node &node, uint32_t indent);
        void write_flow_node(const Node &node);
        void write_block_map(const Node &node, uint32_t indent);
        void write_block_seq(const Node &node, uint32_t indent);
        void write_flow_map(const Node &node);
        void write_flow_seq(const Node &node);
        void write_key(const Node &node, uint32_t indent);
        void write_value(const Node &node, uint32_t indent);
    };

} // namespace cyaml

#endif // CYAML_SERIALIZER_H
