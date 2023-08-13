#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "cyaml.h"
#include "gtest/gtest.h"

class Serializer_Test: public testing::Test
{
public:
    const std::string test_case_dirname = "../test/test_case/serializer_test/";

    static void SetUpTestSuite()
    {
        std::cout << "serializer test start..." << std::endl;
    }

    static void TearDownTestSuite()
    {
        std::cout << "serializer test finish" << std::endl;
    }
};

TEST_F(Serializer_Test, temp)
{
    cyaml::Node node;
    node.push_back(1);
    node.push_back(cyaml::Node());
    node[1]["a"] = 2;
    node[1]["b"] = 3;

    cyaml::Node key;
    key.set_style(cyaml::Node_Style::FLOW);
    key.push_back("c1");
    key.push_back("c2");
    node[1][key].push_back(8);
    node[1][key].push_back(9);
    node[1][key].set_style(cyaml::Node_Style::FLOW);

    node.push_back(cyaml::Node());
    node[2].push_back(4);
    node[2].push_back(5);
    node[2].push_back(6);
    node[2].set_style(cyaml::Node_Style::FLOW);

    node.push_back(7);

    std::cout << node << std::endl;
    cyaml::dump("test.yaml", node);
}

#ifdef CYAML_TEST

#endif

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
