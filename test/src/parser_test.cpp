#include "parser/parser.h"
#include "type/value.h"
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
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

    void parse_test(std::string test_name)
    {
        std::ifstream input_in(test_case_dirname + test_name + ".in");
        ASSERT_TRUE(input_in.is_open());

        cyaml::Parser parser(input_in);
        cyaml::Value value = parser.parse();

        using namespace cyaml::type;
        ASSERT_EQ(value.type(), cyaml::Node_Type::MAP);
        ASSERT_EQ(value.size(), 3);
        ASSERT_EQ(value["scalar"].as<std::string>(), "a");
        ASSERT_FALSE(value.find("seqq"));
        ASSERT_EQ(value["map"].type(), cyaml::Node_Type::MAP);
        ASSERT_EQ(value["seq"].type(), cyaml::Node_Type::SEQUENCE);
        ASSERT_DOUBLE_EQ(value["seq"][1]["seq_map"].as<double>(), 123.0);
        ASSERT_TRUE(value["seq"][2][0].is_null());
        ASSERT_TRUE(value["seq"][2][1].as<Bool>());
        ASSERT_TRUE(value["seq"][2][2].is_null());
    }
};

#ifdef CYAML_DEBUG
TEST_F(Parser_Test, temp)
{
    parse_test("temp");
}
#endif

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
