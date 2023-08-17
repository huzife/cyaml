/**
 * @file    unicode.h
 * @brief   提供 unicode 支持
 * @details 提供 UTF 类型、UTF 检测、编码转换功能
 * @date    2023-8-16
 */

#ifndef CYAML_UNICODE_H
#define CYAML_UNICODE_H

#include <istream>
#include <vector>

namespace cyaml
{
    namespace utf
    {
        /**
         * @enum    utf 编码类型
         */
        enum Type
        {
            UTF_8,
            UTF_16_BE,
            UTF_16_LE,
            UTF_32_BE,
            UTF_32_LE
        };

        /**
         * @enum    utf 文件头检测状态
         */
        enum Intro_State
        {
            S_START,
            S_UTFBE_BOM1,
            S_UTF32BE_BOM2,
            S_UTF32BE_BOM3,
            S_UTF32BE,
            S_UTF16BE,
            S_UTF16BE_BOM1,
            S_UTFLE_BOM1,
            S_UTF16LE_BOM2,
            S_UTF32LE_BOM3,
            S_UTF16LE,
            S_UTF32LE,
            S_UTF8_IMP,
            S_UTF16LE_IMP,
            S_UTF32LE_IMP3,
            S_UTF8_BOM1,
            S_UTF8_BOM2,
            S_UTF8,
            S_ERROR
        };

        /**
         * @enum    文件头检测字节
         */
        enum Intro_Byte
        {
            B_00,
            B_BB,
            B_BF,
            B_EF,
            B_FE,
            B_FF,
            B_ASCII,
            B_OTHER,
            B_MAX
        };
    } // namespace utf

    /**
     * @class   Unicode
     * @brief   提供 unicode 相关功能
     */
    class Unicode
    {
    public:
        static constexpr uint32_t replace_code = 0xFFFD;

    public:
        static utf::Intro_Byte get_intro_byte(int ch);

        /**
         * @brief   检查文件 utf 编码类型
         * @param   input   输入流
         * @return  utf::Type
        */
        static utf::Type check_type(std::istream &input);

        /**
         * @brief   计算 utf8 字符长度
         * @param   byte    utf8 首字节
         * @return  uint32_t
         * @retval  0:      长度错误，可能传入的 utf8 首字节不正确
        */
        static uint32_t get_utf8_len(uint8_t byte);

        /**
         * @brief   将 Unicode 编码为 utf
         * @param   code    Unicode 字符编码
         * @param   type    utf 编码类型
         * @return  std::vector<uint8_t>
         */
        static std::vector<uint8_t> encode(uint32_t code, utf::Type type);

        /**
         * @brief   将 utf 解码为 Unicode
         * @param   bytes   utf 编码字节数组
         * @param   type    utf 编码类型
         * @return  uint32_t
         */
        static uint32_t decode(std::vector<uint8_t> bytes, utf::Type type);

    private:
        static std::vector<uint8_t> encode_to_utf8(uint32_t code);
        static std::vector<uint8_t> encode_to_utf16(uint32_t code);
        static std::vector<uint8_t> encode_to_utf32(uint32_t code);

        static uint32_t decode_from_utf8(std::vector<uint8_t> bytes);
        static uint32_t decode_from_utf16(std::vector<uint8_t> bytes);
        static uint32_t decode_from_utf32(std::vector<uint8_t> bytes);
    };
} // namespace cyaml

#endif // CYAML_UNICODE_H
