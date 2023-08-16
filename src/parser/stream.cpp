/**
 * @file    stream.cpp
 * @brief   输入流源文件
 * @details 用于封装标准输入流，实现对 unicode 的支持
 * @date    2023-8-16
 */

#include "parser/stream.h"
#include <assert.h>
#include <iostream>

namespace cyaml
{
    Stream::Stream(std::istream &input, utf::Type default_type)
        : input_(input),
          type_(Unicode::get_type(input, default_type))
    {
        read_to(1);
    }

    char Stream::get()
    {
        read_to(2);

        if (chars_.size() == 0)
            return eof;

        char ret = chars_.front();
        chars_.pop_front();
        check(ret);
        return ret;
    }

    char Stream::peek() const
    {
        if (chars_.empty())
            return eof;

        return chars_.front();
    }

    bool Stream::read_to(uint32_t count)
    {
        while (input_.good() && chars_.size() < count) {
            read();
        }

        return chars_.size() >= count;
    }

    void Stream::check(char ch)
    {
        switch (ch) {
        case '\n':
            mark_.line++;
            mark_.column = 1;
            break;

        default:
            mark_.column++;
            break;
        }
    }

    void Stream::push(std::vector<uint8_t> chars)
    {
        for (auto byte : chars) {
            chars_.push_back(static_cast<char>(byte));
        }
    }

    void Stream::read()
    {
        switch (type_) {
        case utf::UTF_8:
            read_utf8();
            break;
        case utf::UTF_16_LE:
        case utf::UTF_16_BE:
            read_utf16();
            break;
        case utf::UTF_32_LE:
        case utf::UTF_32_BE:
            read_utf32();
            break;
        }
    }

    void Stream::read_utf8()
    {
        uint8_t byte = input_.get();
        chars_.push_back(static_cast<char>(byte));
    }

    void Stream::read_utf16()
    {
        uint32_t ch = 0;
        std::vector<uint8_t> bytes(2);

        bytes[0] = input_.get();
        bytes[1] = input_.get();

        if (!input_.good())
            return;

        // 小端字节序
        if (type_ == utf::UTF_16_LE) {
            std::swap(bytes[0], bytes[1]);
        }

        ch = (static_cast<uint32_t>(bytes[0]) << 8) |
             (static_cast<uint32_t>(bytes[1]));

        // 前导代理，需要再读两个字节
        if (ch >= 0xD800 && ch < 0xDC00) {
            while (true) {
                std::vector<uint8_t> low_bytes(2);
                low_bytes[0] = input_.get();
                low_bytes[1] = input_.get();

                if (!input_.good()) {
                    push(Unicode::encode(Unicode::replace_code, utf::UTF_8));
                    return;
                }

                // 小端字节序
                if (type_ == utf::UTF_16_LE) {
                    std::swap(low_bytes[0], low_bytes[1]);
                }

                uint32_t low_ch = (static_cast<uint32_t>(low_bytes[0]) << 8) |
                                  (static_cast<uint32_t>(low_bytes[1]));

                // 读到后尾代理，则四个字节组成完整 UTF16 编码
                if (low_ch >= 0xDC00 && low_ch < 0xE000) {
                    bytes.push_back(low_bytes[0]);
                    bytes.push_back(low_bytes[1]);
                    break;
                }

                // 不是后尾代理，前两个字节替换为错误码
                push(Unicode::encode(Unicode::replace_code, utf::UTF_8));

                // 后两个字节是一个完整 UTF16 编码
                if (low_ch < 0xD800 || low_ch >= 0xE000) {
                    uint32_t code = Unicode::decode(low_bytes, utf::UTF_16_BE);
                    push(Unicode::encode(code, utf::UTF_8));
                    return;
                }

                // 剩余情况为后两个字节是一个前导代理码元，继续读后尾代理
                ch = low_ch;
                bytes[0] = low_bytes[0];
                bytes[1] = low_bytes[1];
            }
        }

        uint32_t code = Unicode::decode(bytes, utf::UTF_16_BE);
        push(Unicode::encode(code, utf::UTF_8));
    }

    void Stream::read_utf32()
    {
        std::vector<uint8_t> bytes(4);
        for (int i = 0; i < 4; i++) {
            bytes[i] = input_.get();
        }

        if (!input_.good())
            return;

        uint32_t code = Unicode::decode(bytes, type_);
        push(Unicode::encode(code, utf::UTF_8));
    }

} // namespace cyaml
