/**
 * @file        api.cpp
 * @brief       cyaml 全局 API
 * @details     主要包含 yaml 文件读写接口实现
 * @date        2023-8-13
 */

#include "cyaml/parser/api.h"
#include "cyaml/parser/node_builder.h"
#include "cyaml/parser/parser.h"
#include "cyaml/parser/serializer.h"
#include "cyaml/error/exceptions.h"
#include <fstream>
#include <sstream>

namespace cyaml
{
    Node load(std::istream &input)
    {
        Node_Builder builder;
        Parser(input, builder).parse_next_document();
        return builder.root();
    }

    Node load(const std::string &input)
    {
        std::stringstream ss(input);
        Node_Builder builder;
        Parser(ss, builder).parse_next_document();
        return builder.root();
    }

    Node load(const char *input)
    {
        std::stringstream ss(input);
        Node_Builder builder;
        Parser(ss, builder).parse_next_document();
        return builder.root();
    }

    Node load_file(const std::string &file)
    {
        std::ifstream ifs(file);

        if (!ifs.is_open()) {
            throw Exception("Failed to open \"" + file + "\"", Mark());
        }

        Node_Builder builder;
        Parser(ifs, builder).parse_next_document();
        Node ret = builder.root();
        ifs.close();

        return ret;
    }

    std::vector<Node> load_all(std::istream &input)
    {
        std::vector<Node> nodes;
        Node_Builder builder;
        Parser parser(input, builder);
        while (parser.parse_next_document()) {
            nodes.push_back(builder.root());
        }

        return nodes;
    }

    std::vector<Node> load_all(const std::string &input)
    {
        std::vector<Node> nodes;
        Node_Builder builder;
        std::stringstream ss(input);
        Parser parser(ss, builder);
        while (parser.parse_next_document()) {
            nodes.push_back(builder.root());
        }

        return nodes;
    }

    std::vector<Node> load_all(const char *input)
    {
        std::vector<Node> nodes;
        Node_Builder builder;
        std::stringstream ss(input);
        Parser parser(ss, builder);
        while (parser.parse_next_document()) {
            nodes.push_back(builder.root());
        }

        return nodes;
    }

    std::vector<Node> load_file_all(const std::string &file)
    {
        std::ifstream ifs(file);

        if (!ifs.is_open()) {
            throw Exception("Failed to open \"" + file + "\"", Mark());
        }

        std::vector<Node> nodes;
        Node_Builder builder;
        Parser parser(ifs, builder);
        while (parser.parse_next_document()) {
            nodes.push_back(builder.root());
        }
        ifs.close();

        return nodes;
    }

    void dump(const std::string &file, const Node &node)
    {
        std::ofstream ofs(file);

        if (!ofs.is_open()) {
            throw Exception("Failed to open \"" + file + "\"", Mark());
        }

        ofs << node;
        ofs.close();
    }

    std::string dump(const Node &node)
    {
        std::ostringstream oss;
        oss << node;
        return oss.str();
    }

    std::ostream &operator<<(std::ostream &out, const Node &node)
    {
        Serializer(out).serialize(node);
        return out;
    }

} // namespace cyaml
