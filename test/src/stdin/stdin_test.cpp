#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "cyaml.h"
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

    void parse(cyaml::Node &value)
    {
        cyaml::Parser parser(std::cin);
        value = parser.parse();
    }
};

#ifdef CYAML_TEST
TEST_F(Stdin_Test, empty_document1)
{
    cyaml::Node value;
    parse(value);
    EXPECT_EQ(value.size(), 0);
}

TEST_F(Stdin_Test, empty_document2)
{
    cyaml::Node value;
    parse(value);
    EXPECT_EQ(value.size(), 0);
}

TEST_F(Stdin_Test, flow)
{
    cyaml::Node value;
    parse(value);

    EXPECT_EQ(value.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value["flow_map"].size(), 2);
    EXPECT_EQ(value["flow_map"]["key1"].as<std::string>(), "value1");
    EXPECT_EQ(value["flow_seq"].size(), 4);
    EXPECT_EQ(value["flow_seq"][0].as<std::string>(), "1");
    EXPECT_TRUE(value["flow_seq"][2].is_null());
}

TEST_F(Stdin_Test, nested_flow)
{
    cyaml::Node value;
    parse(value);

    EXPECT_EQ(value.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value.size(), 5);
    EXPECT_EQ(value["a"].as<std::string>(), "hello");
    EXPECT_EQ(value["b"].as<std::string>(), "world");
    EXPECT_EQ(value["null"].as<std::string>(), "");
    EXPECT_EQ(value["c"].type(), cyaml::Node_Type::SEQ);
    EXPECT_EQ(value["c"].size(), 4);
    EXPECT_EQ(value["c"][0].as<int>(), 1);
    EXPECT_TRUE(value["c"][2].is_null());
    EXPECT_EQ(value["c"][3].type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value["c"][3]["3"].as<std::string>(), "map in flow_seq");
    EXPECT_TRUE(value["key"].is_null());
}

TEST_F(Stdin_Test, flow_in_line)
{
    cyaml::Node value;
    parse(value);

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

TEST_F(Stdin_Test, value)
{
    cyaml::Node value;
    parse(value);

    EXPECT_EQ(value.type(), cyaml::Node_Type::MAP);
    EXPECT_EQ(value.size(), 3);
    EXPECT_EQ(value["scalar"].as<std::string>(), "a");
    EXPECT_FALSE(value.contain("seqq"));
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
