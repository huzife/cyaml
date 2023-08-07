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
        EXPECT_TRUE(input_in.is_open());

        cyaml::Parser parser(input_in);
        value = parser.parse();
    }
};

#ifdef CYAML_TEST
TEST_F(Parser_Test, anchor_alias)
{
    cyaml::Value value;
    parse("anchor_alias", value);
    EXPECT_EQ(value.size(), 3);
    EXPECT_EQ(value["a"].as<int>(), 1);
    EXPECT_EQ(value["b"]["b1"].as<int>(), 2);
    EXPECT_EQ(value["b"]["b2"].as<int>(), 1);
    EXPECT_TRUE(value.find("{b1: 2, b2: 1}"));
    EXPECT_EQ(value["{b1: 2, b2: 1}"].as<int>(), 3);

    cyaml::Value b = value["b"];
    EXPECT_TRUE(value.find(b));
    EXPECT_EQ(value[b].as<int>(), 3);
}

TEST_F(Parser_Test, complex_key)
{
    cyaml::Value value;
    parse("complex_key", value);
    EXPECT_EQ(value.size(), 2);
    EXPECT_TRUE(value.find("{a: 1, b: 2}"));
    EXPECT_TRUE(value.find("[4, 5]"));
    EXPECT_EQ(value["{a: 1, b: 2}"][0].as<int>(), 3);
    EXPECT_TRUE(value["{a: 1, b: 2}"][1].is_null());
    value["{a: 1, b: 2}"][1] = 2;
    EXPECT_EQ(value["{a: 1, b: 2}"][1].as<std::string>(), "2");
    EXPECT_EQ(value["[4, 5]"]["c"].as<int>(), 6);
    EXPECT_EQ(value["[4, 5]"]["d"].as<int>(), 7);
}

TEST_F(Parser_Test, empty_document1)
{
    cyaml::Value value;
    parse("empty_document1", value);
    EXPECT_EQ(value.size(), 0);
}

TEST_F(Parser_Test, empty_document2)
{
    cyaml::Value value;
    parse("empty_document2", value);
    EXPECT_EQ(value.size(), 0);
}

TEST_F(Parser_Test, flow)
{
    cyaml::Value value;
    parse("flow", value);

    EXPECT_EQ(value.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value["flow_map"].size(), 2);
    EXPECT_EQ(value["flow_map"]["key1"].as<std::string>(), "value1");
    EXPECT_EQ(value["flow_seq"].size(), 4);
    EXPECT_EQ(value["flow_seq"][0].as<std::string>(), "1");
    EXPECT_TRUE(value["flow_seq"][2].is_null());
}

TEST_F(Parser_Test, nested_flow)
{
    cyaml::Value value;
    parse("nested_flow", value);

    EXPECT_EQ(value.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value.size(), 5);
    EXPECT_EQ(value["a"].as<std::string>(), "hello");
    EXPECT_EQ(value["b"].as<std::string>(), "world");
    EXPECT_EQ(value["null"].as<std::string>(), "null");
    EXPECT_EQ(value["c"].type(), cyaml::Node_Type::SEQ);
    EXPECT_EQ(value["c"].size(), 4);
    EXPECT_EQ(value["c"][0].as<int>(), 1);
    EXPECT_TRUE(value["c"][2].is_null());
    EXPECT_EQ(value["c"][3].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value["c"][3]["3"].as<std::string>(), "map in flow_seq");
    EXPECT_TRUE(value["key"].is_null());
}

TEST_F(Parser_Test, flow_in_line)
{
    cyaml::Value value;
    parse("flow_in_line", value);

    EXPECT_EQ(value["a"].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value["a"]["a1"].as<std::string>(), "hello");
    EXPECT_EQ(value["a"]["a2"].type(), cyaml::Node_Type::SEQ);
    EXPECT_EQ(value["a"]["a2"][0].as<std::string>(), "a21");
    EXPECT_EQ(value["a"]["a2"][1].as<std::string>(), "a22");
    EXPECT_TRUE(value["a"]["a2"][2].is_null());
    EXPECT_TRUE(value["a"]["a3"].is_null());
    EXPECT_EQ(value["b"].type(), cyaml::Node_Type::SEQ);
    EXPECT_EQ(value["b"][0].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value["b"][0]["b1"]["b11"].as<std::string>(), "world");
}

TEST_F(Parser_Test, value)
{
    cyaml::Value value;
    parse("value", value);

    EXPECT_EQ(value.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value.size(), 3);
    EXPECT_EQ(value["scalar"].as<std::string>(), "a");
    EXPECT_FALSE(value.find("seqq"));
    EXPECT_EQ(value["map"].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value["seq"].type(), cyaml::Node_Type::SEQ);
    EXPECT_DOUBLE_EQ(value["seq"][1]["seq_map"].as<float>(), 123.0);
    EXPECT_TRUE(value["seq"][2][0].is_null());
    EXPECT_TRUE(value["seq"][2][1].as<bool>());
    EXPECT_TRUE(value["seq"][2][2].is_null());
}
#endif

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
