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
        Mark mark_ = Mark(1, 1);      // 当前输出位置

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
         * @brief   判断是否位于起始位置
         * @return  bool
         */
        bool on_begin() const
        {
            return mark_.line == 1 && mark_.column == 1;
        }

        /**
         * @brief   填充空白字符到目标位置
         * @param   Mark    目标位置
         * @return  void
         */
        void fill_blank_to(Mark dest);

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

        void write_map(const Node &node, uint32_t indent);
        void write_seq(const Node &node, uint32_t indent);
    };

} // namespace cyaml

#endif // CYAML_SERIALIZER_H
