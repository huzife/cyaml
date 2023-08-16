/**
 * @file    stream.cpp
 * @brief   输入流源文件
 * @details 用于封装标准输入流，实现对 unicode 的支持
 * @date    2023-8-16
 */

#include "parser/stream.h"
#include <assert.h>

namespace cyaml
{
    Stream::Stream(std::istream &input): input_stream_(input)
    {
        chars_.push_back(input_stream_.get());
    }

    char Stream::get()
    {
        if (!input_stream_.eof()) {
            chars_.push_back(input_stream_.get());
        }
        char ret = chars_.front();

        check(ret);

        return ret;
    }

    char Stream::next() const
    {
        assert(!chars_.empty());
        return chars_.front();
    }

    bool Stream::read(uint32_t count)
    {
        while (chars_.size() < count && !input_stream_.eof()) {
            chars_.push_back(input_stream_.get());
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

        case -1:
            stream_end_ = true;
            break;

        default:
            mark_.column++;
            break;
        }

        if (!stream_end_) {
            chars_.pop_front();
        }
    }

} // namespace cyaml
