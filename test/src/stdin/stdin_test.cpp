#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "cyaml/cyaml.h"
#include "gtest/gtest.h"

class Stdin_Test: public testing::Test
{
public:
    static void SetUpTestSuite()
    {
        std::cout << "parser test start..." << std::endl;
    }

    static void TearDownTestSuite()
    {
        std::cout << "parser test finish" << std::endl;
    }

    void parse(cyaml::Node &node)
    {
        node = cyaml::load(std::cin);
    }
};

TEST_F(Stdin_Test, empty_document1)
{
    cyaml::Node node;
    parse(node);
    EXPECT_EQ(node.size(), 0);
}

TEST_F(Stdin_Test, empty_document2)
{
    cyaml::Node node;
    parse(node);
    EXPECT_EQ(node.size(), 0);
}

TEST_F(Stdin_Test, flow)
{
    cyaml::Node node;
    parse(node);

    EXPECT_EQ(node.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node["flow_map"].size(), 2);
    EXPECT_EQ(node["flow_map"]["key1"].as<std::string>(), "value1");
    EXPECT_EQ(node["flow_seq"].size(), 4);
    EXPECT_EQ(node["flow_seq"][0].as<std::string>(), "1");
    EXPECT_TRUE(node["flow_seq"][2].is_null());
}

TEST_F(Stdin_Test, nested_flow)
{
    cyaml::Node node;
    parse(node);

    EXPECT_EQ(node.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node.size(), 5);
    EXPECT_EQ(node["a"].as<std::string>(), "hello");
    EXPECT_EQ(node["b"].as<std::string>(), "world");
    EXPECT_EQ(node["null"].as<std::string>(), "");
    EXPECT_EQ(node["c"].type(), cyaml::Node_Type::SEQ);
    EXPECT_EQ(node["c"].size(), 4);
    EXPECT_EQ(node["c"][0].as<int>(), 1);
    EXPECT_TRUE(node["c"][2].is_null());
    EXPECT_EQ(node["c"][3].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(node["c"][3]["3"].as<std::string>(), "map in flow_seq");
    EXPECT_TRUE(node["key"].is_null());
}

TEST_F(Stdin_Test, flow_in_line)
{
    cyaml::Node node;
    parse(node);

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

TEST_F(Stdin_Test, node)
{
    cyaml::Node node;
    parse(node);

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

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
