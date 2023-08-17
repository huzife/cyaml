/**
 * @file    stream.h
 * @brief   输入流
 * @details 用于封装标准输入流，实现对 unicode 的支持
 * @date    2023-8-16
 */

#ifndef CYAML_STREAM_H
#define CYAML_STREAM_H

#include "type/mark.h"
#include "parser/unicode.h"
#include <istream>
#include <deque>

namespace cyaml
{
    /**
     * @class   Stream
     * @brief   输入流
     */
    class Stream
    {
    public:
        static constexpr char eof = -1;

    private:
        std::istream &input_;
        std::deque<char> chars_;
        utf::Type type_;

        Mark mark_{1, 1};

    public:
        /**
         * @brief   Stream 类构造函数
         * @param   input           标准输入流
         */
        Stream(std::istream &input);

        /**
         * @brief   判断输入流状态
         * @return  bool
         */
        operator bool() const
        {
            return input_.good() || !chars_.empty();
        }

        bool operator!() const
        {
            return !static_cast<bool>(*this);
        }

        /**
         * @brief   读取下一个字符
         * @return  char
         */
        char get();

        /**
         * @brief   查看下一个字符
         * @return  char
         */
        char peek() const;

        /**
         * @brief   获取当前位置
         * @return  Mark
         */
        Mark mark() const
        {
            return mark_;
        }

        /**
         * @brief   获取当前行号
         * @return  uint32_t
         */
        uint32_t line() const
        {
            return mark_.line;
        }

        /**
         * @brief   获取当前行号
         * @return  uint32_t
         */
        uint32_t column() const
        {
            return mark_.column;
        }

        /**
         * @brief   字符队列补充到指定个数字符
         * @param   count   字符数
         * @return  bool
         * @retval  true:   能够获取足够字符
         * @retval  false:  没有足够字符
         */
        bool read_to(uint32_t count);

        /**
         * @brief   获取指定位置的字符
         * @param   index   需要获取的字符下标
         * @return  char
         */
        char at(uint32_t index) const
        {
            return chars_[index];
        }

    private:
        /**
         * @brief   处理字符
         * @param   ch      待处理字符
         * @return  void
         */
        void check(char ch);

        /**
         * @brief   添加一组字符
         * @param   chars   字符数组
         * @return  void
         */
        void push(std::vector<uint8_t> chars);

        /**
         * @brief   读一个 unicode 字符
         * @return  void
         */
        void read();

        /**
         * @brief   读 utf8 字符
         * @return  void
         */
        void read_utf8();

        /**
         * @brief   读 utf8 字符
         * @return  void
         */
        void read_utf16();

        /**
         * @brief   读 utf8 字符
         * @return  void
         */
        void read_utf32();
    };
} // namespace cyaml

#endif // CYAML_STREAM_H
