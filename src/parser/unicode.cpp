/**
 * @file    unicode.cpp
 * @brief   提供 unicode 支持
 * @details 提供 UTF 类型、UTF 检测、编码转换功能
 * @date    2023-8-16
 */

#include "cyaml/parser/unicode.h"
#include <assert.h>
#include <algorithm>

namespace cyaml
{
    using namespace utf;

    static bool final_state[] = {
            false, // S_START
            false, // S_UTFBE_BOM1
            false, // S_UTF32BE_BOM2
            false, // S_UTF32BE_BOM3
            true,  // S_UTF32BE
            true,  // S_UTF16BE
            false, // S_UTF16BE_BOM1
            false, // S_UTFLE_BOM1
            false, // S_UTF16LE_BOM2
            false, // S_UTF32LE_BOM3
            true,  // S_UTF16LE
            true,  // S_UTF32LE
            false, // S_UTF8_IMP
            false, // S_UTF16LE_IMP
            false, // S_UTF32LE_IMP3
            false, // S_UTF8_BOM1
            false, // S_UTF8_BOM2
            true,  // S_UTF8
            true,  // S_ERROR
    };

    static Intro_State transitions[][B_MAX] = {
            // B_00,           B_BB,           B_BF,           B_EF,
            // B_FE,           B_FF,           B_ASCII,        B_OTHER
            {S_UTFBE_BOM1, S_UTF8, S_UTF8, S_UTF8_BOM1, S_UTF16BE_BOM1,
             S_UTFLE_BOM1, S_UTF8_IMP, S_UTF8},
            {S_UTF32BE_BOM2, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF16BE,
             S_UTF8},
            {S_UTF32BE, S_UTF8, S_UTF8, S_UTF8, S_UTF32BE_BOM3, S_UTF8, S_UTF8,
             S_UTF8},
            {S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF32BE, S_UTF8,
             S_UTF8},
            {S_UTF32BE, S_UTF32BE, S_UTF32BE, S_UTF32BE, S_UTF32BE, S_UTF32BE,
             S_UTF32BE, S_UTF32BE},
            {S_UTF16BE, S_UTF16BE, S_UTF16BE, S_UTF16BE, S_UTF16BE, S_UTF16BE,
             S_UTF16BE, S_UTF16BE},
            {S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF16BE, S_UTF8,
             S_UTF8},
            {S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF16LE_BOM2, S_UTF8, S_UTF8,
             S_UTF8},
            {S_UTF32LE_BOM3, S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE,
             S_UTF16LE, S_UTF16LE, S_UTF16LE},
            {S_UTF32LE, S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE,
             S_UTF16LE, S_UTF16LE},
            {S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE,
             S_UTF16LE, S_UTF16LE},
            {S_UTF32LE, S_UTF32LE, S_UTF32LE, S_UTF32LE, S_UTF32LE, S_UTF32LE,
             S_UTF32LE, S_UTF32LE},
            {S_UTF16LE_IMP, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8,
             S_UTF8},
            {S_UTF32LE_IMP3, S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE,
             S_UTF16LE, S_UTF16LE, S_UTF16LE},
            {S_UTF32LE, S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE, S_UTF16LE,
             S_UTF16LE, S_UTF16LE},
            {S_UTF8, S_UTF8_BOM2, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8,
             S_UTF8},
            {S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8},
            {S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8, S_UTF8},
    };

    static char unget_count[][B_MAX] = {
            {0, 1, 1, 0, 0, 0, 0, 1}, {0, 2, 2, 2, 2, 2, 2, 2},
            {3, 3, 3, 3, 0, 3, 3, 3}, {4, 4, 4, 4, 4, 0, 4, 4},
            {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1},
            {2, 2, 2, 2, 2, 0, 2, 2}, {2, 2, 2, 2, 0, 2, 2, 2},
            {0, 1, 1, 1, 1, 1, 1, 1}, {0, 2, 2, 2, 2, 2, 2, 2},
            {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1},
            {0, 2, 2, 2, 2, 2, 2, 2}, {0, 3, 3, 3, 3, 3, 3, 3},
            {4, 4, 4, 4, 4, 4, 4, 4}, {2, 0, 2, 2, 2, 2, 2, 2},
            {3, 3, 0, 3, 3, 3, 3, 3}, {1, 1, 1, 1, 1, 1, 1, 1},
    };

    Intro_Byte Unicode::get_intro_byte(int ch)
    {
        if (ch == -1)
            return B_OTHER;

        switch (ch) {
        case 0:
            return B_00;
        case 0xBB:
            return B_BB;
        case 0xBF:
            return B_BF;
        case 0xEF:
            return B_EF;
        case 0xFE:
            return B_FE;
        case 0xFF:
            return B_FF;
        }

        if (ch > 0 && ch < 0xFF)
            return B_ASCII;

        return B_OTHER;
    }

    utf::Type Unicode::check_type(std::istream &input)
    {
        int intro[4]{};
        int intro_used = 0;
        Intro_State state = S_START;
        while (!final_state[state]) {
            int ch = input.get();
            intro[intro_used++] = ch;
            Intro_Byte byte = get_intro_byte(ch);
            Intro_State new_state = transitions[state][byte];
            int unget = unget_count[state][byte];
            if (unget > 0) {
                input.clear();
                while (unget > 0) {
                    if (intro[--intro_used] != -1) {
                        input.putback(static_cast<char>(intro[intro_used]));
                    }
                    unget--;
                }
            }
            state = new_state;
        }

        switch (state) {
        case S_UTF8:
            return UTF_8;
        case S_UTF16LE:
            return UTF_16_LE;
        case S_UTF16BE:
            return UTF_16_BE;
        case S_UTF32LE:
            return UTF_32_LE;
        case S_UTF32BE:
            return UTF_32_BE;
        }

        return UTF_8;
    }

    uint32_t Unicode::get_utf8_len(uint8_t byte)
    {
        uint32_t len = 0;
        uint8_t mask = 0x80;
        while (byte & mask) {
            len++;
            mask >>= 1;
        }

        if (len == 0) {
            len = 1;
        } else if (len == 1 || len > 4) {
            len = 0;
        }

        return len;
    }

    std::vector<uint8_t> Unicode::encode(uint32_t code, utf::Type type)
    {
        std::vector<uint8_t> bytes;
        bool need_reverse = false;

        switch (type) {
        case UTF_8:
            bytes = encode_to_utf8(code);
            break;
        case UTF_16_LE:
            need_reverse = true;
        case UTF_16_BE:
            bytes = encode_to_utf16(code);
            break;
        case UTF_32_LE:
            need_reverse = true;
        case UTF_32_BE:
            bytes = encode_to_utf32(code);
            break;
        }

        if (need_reverse) {
            std::reverse(bytes.begin(), bytes.end());
        }
        return bytes;
    }

    uint32_t Unicode::decode(std::vector<uint8_t> bytes, utf::Type type)
    {
        uint32_t code = 0;
        switch (type) {
        case UTF_8:
            code = decode_from_utf8(bytes);
            break;
        case UTF_16_LE:
            std::reverse(bytes.begin(), bytes.end());
        case UTF_16_BE:
            code = decode_from_utf16(bytes);
            break;
        case UTF_32_LE:
            std::reverse(bytes.begin(), bytes.end());
        case UTF_32_BE:
            code = decode_from_utf32(bytes);
            break;
        }

        return code;
    }

    std::vector<uint8_t> Unicode::encode_to_utf8(uint32_t code)
    {
        if (code < 0x80)
            return {static_cast<uint8_t>(code)};

        std::vector<uint8_t> bytes;
        while (code >= 0x20) {
            bytes.push_back((code & 0x3F) | 0x80);
            code >>= 6;
        }

        size_t size = bytes.size() + 1;
        uint8_t header = (((1 << size) - 1) << (8 - size));
        uint8_t mask = (0xFF >> (size + 1));
        bytes.push_back(header | (code & mask));

        std::reverse(bytes.begin(), bytes.end());
        return bytes;
    }

    std::vector<uint8_t> Unicode::encode_to_utf16(uint32_t code)
    {
        std::vector<uint8_t> bytes;
        if (code < 0x10000) {
            bytes.resize(2);
            bytes[0] = static_cast<uint8_t>((code >> 8) & 0xFF);
            bytes[1] = static_cast<uint8_t>(code & 0xFF);
        } else {
            bytes.resize(4);
            bytes[0] = static_cast<uint8_t>((code >> 18) & 0x3);
            bytes[1] = static_cast<uint8_t>((code >> 10) & 0xFF);
            bytes[2] = static_cast<uint8_t>((code >> 8) & 0x3);
            bytes[3] = static_cast<uint8_t>(code & 0xFF);
        }

        return bytes;
    }

    std::vector<uint8_t> Unicode::encode_to_utf32(uint32_t code)
    {
        std::vector<uint8_t> bytes(4);
        std::for_each(bytes.rbegin(), bytes.rend(), [&](uint8_t &i) {
            i = static_cast<uint8_t>(code & 0xFF);
            code >>= 8;
        });

        return bytes;
    }

    uint32_t Unicode::decode_from_utf8(std::vector<uint8_t> bytes)
    {
        assert(!bytes.empty());

        auto len = get_utf8_len(bytes[0]);

        if (len == 1)
            return bytes[0];

        assert(bytes.size() == len);
        uint32_t ret = 0;
        ret = bytes[0] & (0xFF >> (len + 1));

        for (int i = 1; i < len; i++) {
            ret <<= 6;
            ret |= (bytes[i] & 0x3F);
        }

        return ret;
    }

    uint32_t Unicode::decode_from_utf16(std::vector<uint8_t> bytes)
    {
        assert(bytes.size() == 2 || bytes.size() == 4);

        uint32_t ch = (static_cast<uint32_t>(bytes[0]) << 8) |
                      (static_cast<uint32_t>(bytes[1]));

        uint32_t code = 0;
        if (ch >= 0xDC00 && ch < 0xE000)
            return REPLACE_CODE;

        if (ch < 0xD800 || ch >= 0xE000) {
            // 一次只能解码一个
            assert(bytes.size() == 2);
            return ch;
        }

        if (bytes.size() == 2)
            return REPLACE_CODE;

        uint32_t low_ch = (static_cast<uint32_t>(bytes[2]) << 8) |
                          (static_cast<uint32_t>(bytes[3]));

        // 后尾代理
        assert(low_ch >= 0xDC00 && low_ch < 0xE000);
        code = (((ch & 0x3FF) << 10) | (low_ch & 0x3FF)) + 0x10000;
        return code;
    }

    uint32_t Unicode::decode_from_utf32(std::vector<uint8_t> bytes)
    {
        assert(bytes.size() == 4);
        uint32_t code = 0;
        for (auto i : bytes) {
            code <<= 8;
            code |= i;
        }

        return code;
    }

} // namespace cyaml
