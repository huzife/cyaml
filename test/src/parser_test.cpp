#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "cyaml.h"
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

    void parse(std::string test_name, cyaml::Node &node)
    {
        std::ifstream input_in(test_case_dirname + test_name + ".in");
        EXPECT_TRUE(input_in.is_open());

        cyaml::Parser parser(input_in);
        node = parser.parse();
    }
};

#ifdef CYAML_TEST
TEST_F(Parser_Test, anchor_alias)
{
    cyaml::Node node;
    parse("anchor_alias", node);
    EXPECT_EQ(node.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node.size(), 4);
    EXPECT_EQ(node["a"].as<int>(), 1);
    EXPECT_EQ(node["b"]["b1"].as<int>(), 2);
    EXPECT_EQ(node["b"]["b2"].as<int>(), 1);

    cyaml::Node b = node["b"];
    ASSERT_TRUE(node.contain(b));
    EXPECT_EQ(node[b].as<int>(), 3);

    b["b3"] = 3;
    ASSERT_TRUE(node["b"].contain("b3"));
    EXPECT_EQ(node["b"]["b3"].as<int>(), 3);

    cyaml::Node bb;
    bb["b1"] = 2;
    bb["b2"] = 1;
    bb["b3"] = 3;
    ASSERT_TRUE(node.contain(bb));
    EXPECT_EQ(node[bb].as<int>(), 3);

    cyaml::Node c;
    c["c1"] = 1;
    c["c2"] = 2;
    ASSERT_TRUE(node.contain(c));
    EXPECT_EQ(node[c].as<int>(), 4);
}

TEST_F(Parser_Test, complex_key)
{
    cyaml::Node node;
    parse("complex_key", node);
    EXPECT_EQ(node.size(), 2);
    // EXPECT_TRUE(node.contain("{a: 1, b: 2}"));
    // EXPECT_TRUE(node.contain("[4, 5]"));
    // EXPECT_EQ(node["{a: 1, b: 2}"][0].as<int>(), 3);
    // EXPECT_TRUE(node["{a: 1, b: 2}"][1].is_null());
    // node["{a: 1, b: 2}"][1] = 2;
    // EXPECT_EQ(node["{a: 1, b: 2}"][1].as<std::string>(), "2");
    // EXPECT_EQ(node["[4, 5]"]["c"].as<int>(), 6);
    // EXPECT_EQ(node["[4, 5]"]["d"].as<int>(), 7);
}

TEST_F(Parser_Test, empty_document1)
{
    cyaml::Node node;
    parse("empty_document1", node);
    EXPECT_EQ(node.size(), 0);
}

TEST_F(Parser_Test, empty_document2)
{
    cyaml::Node node;
    parse("empty_document2", node);
    EXPECT_EQ(node.size(), 0);
}

TEST_F(Parser_Test, flow)
{
    cyaml::Node node;
    parse("flow", node);
    EXPECT_EQ(node.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node["flow_map"].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node["flow_map"].size(), 2);
    EXPECT_EQ(node["flow_map"]["key1"].as<std::string>(), "value1");
    EXPECT_EQ(node["flow_seq"].size(), 4);
    EXPECT_EQ(node["flow_seq"][0].as<std::string>(), "1");
    EXPECT_TRUE(node["flow_seq"][2].is_null());
}

TEST_F(Parser_Test, nested_flow)
{
    cyaml::Node node;
    parse("nested_flow", node);

    EXPECT_EQ(node.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node.size(), 5);
    EXPECT_EQ(node["a"].as<std::string>(), "hello");
    EXPECT_EQ(node["b"].as<std::string>(), "world");
    EXPECT_EQ(node["null"].as<std::string>(), "null");
    EXPECT_EQ(node["c"].type(), cyaml::Node_Type::SEQ);
    EXPECT_EQ(node["c"].size(), 4);
    EXPECT_EQ(node["c"][0].as<int>(), 1);
    EXPECT_TRUE(node["c"][2].is_null());
    EXPECT_EQ(node["c"][3].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node["c"][3]["3"].as<std::string>(), "map in flow_seq");
    EXPECT_TRUE(node["key"].is_null());
}

TEST_F(Parser_Test, flow_in_line)
{
    cyaml::Node node;
    parse("flow_in_line", node);

    EXPECT_EQ(node["a"].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node["a"]["a1"].as<std::string>(), "hello");
    EXPECT_EQ(node["a"]["a2"].type(), cyaml::Node_Type::SEQ);
    EXPECT_EQ(node["a"]["a2"][0].as<std::string>(), "a21");
    EXPECT_EQ(node["a"]["a2"][1].as<std::string>(), "a22");
    EXPECT_TRUE(node["a"]["a2"][2].is_null());
    EXPECT_TRUE(node["a"]["a3"].is_null());
    EXPECT_EQ(node["b"].type(), cyaml::Node_Type::SEQ);
    EXPECT_EQ(node["b"][0].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node["b"][0]["b1"]["b11"].as<std::string>(), "world");
}

TEST_F(Parser_Test, node)
{
    cyaml::Node node;
    parse("value", node);

    EXPECT_EQ(node.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node.size(), 3);
    EXPECT_EQ(node["scalar"].as<std::string>(), "a");
    EXPECT_FALSE(node.contain("seqq"));
    EXPECT_EQ(node["map"].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node["seq"].type(), cyaml::Node_Type::SEQ);
    EXPECT_DOUBLE_EQ(node["seq"][1]["seq_map"].as<float>(), 123.0);
    EXPECT_TRUE(node["seq"][2][0].is_null());
    EXPECT_TRUE(node["seq"][2][1].as<bool>());
    EXPECT_TRUE(node["seq"][2][2].is_null());
}
#endif

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
