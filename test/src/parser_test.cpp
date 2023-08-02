#include "parser/parser.h"
#include "type/value.h"
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "gtest/gtest.h"

using namespace cyaml::type;

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

    void parse(std::string test_name, cyaml::Value &value)
    {
        std::ifstream input_in(test_case_dirname + test_name + ".in");
        ASSERT_TRUE(input_in.is_open());

        cyaml::Parser parser(input_in);
        value = parser.parse();
    }
};

#ifdef CYAML_TEST
TEST_F(Parser_Test, flow)
{
    cyaml::Value value;
    parse("flow", value);

    ASSERT_EQ(value.type(), cyaml::Node_Type::MAP);
    ASSERT_EQ(value["flow_map"].size(), 2);
    ASSERT_EQ(value["flow_map"]["key1"].as<String>(), "value1");
    ASSERT_EQ(value["flow_seq"].size(), 4);
    ASSERT_EQ(value["flow_seq"][0].as<String>(), "1");
    ASSERT_TRUE(value["flow_seq"][2].is_null());
}

TEST_F(Parser_Test, temp)
{
    cyaml::Value value;
    parse("temp", value);

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
#endif

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
