#include "parser/scanner.h"
#include <iostream>
#include <fstream>
#include <string>
#include "gtest/gtest.h"

class Scanner_Test: public testing::Test
{
public:
    const std::string test_case_dirname = "../test/test_case/scanner_test/";

    static void SetUpTestSuite()
    {
        std::cout << "scanner test start..." << std::endl;
    }

    static void TearDownTestSuite()
    {
        std::cout << "scanner test finish" << std::endl;
    }

    void scan_test(std::string in, std::string out)
    {
        std::ifstream input_in(test_case_dirname + in);
        std::ifstream input_out(test_case_dirname + out);
        ASSERT_TRUE(input_in.is_open());
        ASSERT_TRUE(input_out.is_open());

        std::string out_line;
        std::string token;
        cyaml::Scanner scanner(input_in);
        while (!scanner.end() && !input_out.eof()) {
            std::getline(input_out, out_line, '#');

            // 跳过空行
            if (out_line.empty()) {
                continue;
            }

            if (out_line[0] == '\n')
                out_line = out_line.substr(1);
            token = scanner.next_token().to_string();
            EXPECT_EQ(out_line, token);
        }

        // 清除 .out 文件结尾空行
        out_line.clear();
        while (!input_out.eof() && out_line.empty()) {
            std::getline(input_out, out_line, '#');
        }

        EXPECT_TRUE(scanner.end());
        EXPECT_TRUE(input_out.eof());

        input_in.close();
        input_out.close();
    }
};

#ifdef CYAML_TEST
// 基本 token 解析测试
TEST_F(Scanner_Test, token1)
{
    scan_test("token1.in", "token1.out");
}

TEST_F(Scanner_Test, token2)
{
    scan_test("token2.in", "token2.out");
}

// 带单引号的字符串测试
TEST_F(Scanner_Test, quote_string)
{
    scan_test("quote_string.in", "quote_string.out");
}

// 转义字符测试
TEST_F(Scanner_Test, escape)
{
    scan_test("escape.in", "escape.out");
}

// 多行字符串缩进边界情况测试
TEST_F(Scanner_Test, indent)
{
    scan_test("indent.in", "indent.out");
}

// 保留换行和折叠换行测试
TEST_F(Scanner_Test, special_str)
{
    scan_test("special_str.in", "special_str.out");
}
#endif

#ifdef CYAML_DEBUG
// 临时测试用例
TEST_F(Scanner_Test, temp)
{
    scan_test("temp.in", "temp.out");
}
#endif

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
