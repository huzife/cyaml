/**
 * @file        api.h
 * @brief       cyaml 全局 API
 * @details     主要包含 yaml 文件读写接口
 * @date        2023-8-13
 */

#ifndef CYAML_API_H
#define CYAML_API_H

#include "type/node/node.h"

namespace cyaml
{
    /**
     * @brief   从输入流加载
     * @param   std::istream &  输入流
     * @return  Node
     */
    Node load(std::istream &input);

    /**
     * @brief   从字符串加载
     * @param   std::string     输入字符串
     * @return  Node
     */
    Node load(std::string input);

    /**
     * @brief   从字符串加载
     * @param   const char *    输入字符串
     * @return  Node
     */
    Node load(const char *input);

    /**
     * @brief   从文件加载
     * @param   std::string     文件路径
     * @return  Node
     */
    Node load_file(std::string file);

    /**
     * @brief   输出到输出流
     * @param   std::ostream &  输出流
     * @param   const Node &    节点
     * @return  void
     */
    void dump(std::ostream &out, const Node &node);

    /**
     * @brief   输出到文件
     * @param   std::string     文件路径
     * @param   const Node &    节点
     * @return  void
     */
    void dump(std::string file, const Node &node);

    /**
     * @brief   转换为字符串
     * @param   const Node &    节点
     * @return  std::string
     */
    std::string dump(const Node &node);

    /**
     * @brief   通过标准输出流 << 运算符输出
     * @param   std::ostream &  输出流
     * @param   const Node &    节点
     * @return  std::ostream &
     */
    std::ostream &operator<<(std::ostream &out, const Node &node);

} // namespace cyaml

#endif // CYAML_API_H
