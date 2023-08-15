/**
 * @file        api.cpp
 * @brief       cyaml 全局 API
 * @details     主要包含 yaml 文件读写接口实现
 * @date        2023-8-13
 */

#include "parser/api.h"
#include "parser/node_builder.h"
#include "parser/parser.h"
#include "parser/serializer.h"
#include "error/exceptions.h"
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

    Node load(std::string input)
    {
        std::stringstream ss;
        ss << input;
        Node_Builder builder;
        Parser(ss, builder).parse_next_document();
        return builder.root();
    }

    Node load(const char *input)
    {
        std::stringstream ss;
        ss << input;
        Node_Builder builder;
        Parser(ss, builder).parse_next_document();
        return builder.root();
    }

    Node load_file(std::string file)
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

    void dump(std::ostream &out, const Node &node)
    {
        out << node;
    }

    void dump(std::string file, const Node &node)
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
        Serializer(out) << node;
        return out;
    }

} // namespace cyaml
