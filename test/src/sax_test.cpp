#include <iostream>
#include <fstream>
#include <string>
#include "cyaml/cyaml.h"
#include "gtest/gtest.h"

using namespace cyaml;

class Test_Handler: public cyaml::Event_Handler
{
public:
    std::string output;

    ~Test_Handler() = default;

    virtual void on_document_start(const Mark &) override
    {
        output += "On document start\n";
    }

    virtual void on_document_end() override
    {
        output += "On document end\n";
    }

    virtual void on_map_start(const Mark &, std::string, Node_Style) override
    {
        output += "On map start\n";
    }

    virtual void on_map_end() override
    {
        output += "On map end\n";
    }

    virtual void on_seq_start(const Mark &, std::string, Node_Style) override
    {
        output += "On seq start\n";
    }

    virtual void on_seq_end() override
    {
        output += "On seq end\n";
    }

    virtual void on_scalar(const Mark &, std::string, std::string value)
            override
    {
        output += "On scalar: " + value + '\n';
    }

    virtual void on_null(const Mark &, std::string) override
    {
        output += "On null\n";
    }

    virtual void on_anchor(const Mark &, std::string) override
    {
        output += "On anchor\n";
    }

    virtual void on_alias(const Mark &, std::string) override
    {
        output += "On alias\n";
    }
};

TEST(sax_test, sax_test)
{
    std::ifstream in("../test/test_case/sax_test/json.in");
    std::ifstream out("../test/test_case/sax_test/json.out");
    ASSERT_TRUE(in.is_open());
    ASSERT_TRUE(out.is_open());

    Test_Handler handler;
    Parser parser{in, handler};
    parser.parse_next_document();

    std::string out_str;
    while (!out.eof()) {
        std::string line;
        std::getline(out, line);
        out_str += line + '\n';
    }

    ASSERT_EQ(handler.output, out_str.substr(0, out_str.size() - 1));
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
