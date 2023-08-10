#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include "cyaml.h"
#include "gtest/gtest.h"

class Emitter_Test: public testing::Test
{
public:
    const std::string test_case_dirname = "../test/test_case/emitter_test/";

    static void SetUpTestSuite()
    {
        std::cout << "emitter test start..." << std::endl;
    }

    static void TearDownTestSuite()
    {
        std::cout << "emitter test finish" << std::endl;
    }
};

TEST_F(Emitter_Test, temp)
{
    cyaml::Node node;
    node.push_back(1);
    node.push_back(cyaml::Node());
    node[1]["a"] = 2;
    node[1]["b"] = 3;
    cyaml::Node key;
    key.push_back("c1");
    key.push_back("c2");
    node[1][key].push_back(8);
    node[1][key].push_back(9);
    node.push_back(cyaml::Node());
    node[2].push_back(4);
    node[2].push_back(5);
    node[2].push_back(6);
    node.push_back(7);

    cyaml::Serializer out(std::cout);
    out << node;
    std::cout << std::endl;
}

#ifdef CYAML_TEST

#endif

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
