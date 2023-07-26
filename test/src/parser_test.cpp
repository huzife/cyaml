#include "parser/parser.h"
#include "type/value.h"
#include <iostream>
#include <fstream>
#include <string>
#include "gtest/gtest.h"

class Parser_Test: public testing::Test
{
public:
    const std::string test_case_dirname = "../test/test_case/parser_test/";

    static void SetUpTestSuite()
    {
        std::cout << "parser test start..." << std::endl;
    }

    static void TearDownTestSuite()
    {
        std::cout << "parser test finish" << std::endl;
    }

    void parse_test(std::string in, std::string out)
    {
        std::ifstream input_in(test_case_dirname + in);
        ASSERT_TRUE(input_in.is_open());

        cyaml::Parser parser(input_in);
        cyaml::Value value = parser.parse();

        EXPECT_EQ(value.type(), cyaml::Data_Type::SCALAR);
    }
};

TEST_F(Parser_Test, temp)
{
    parse_test("temp.in", "temp.out");
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
