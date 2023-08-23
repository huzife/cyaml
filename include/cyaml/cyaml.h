/**
 * @file        cyaml.h
 * @brief       cyaml 接口头文件
 * @details     用于包含使用 cyaml 所需要的头文件
 * @date        2023-8-9
 */

#ifndef CYAML_H
#define CYAML_H

// error
#include "cyaml/error/exceptions.h"

// event
#include "cyaml/event/event.h"

// parser
#include "cyaml/parser/serializer.h"
#include "cyaml/parser/parser.h"
#include "cyaml/parser/api.h"

// node
#include "cyaml/type/node/node.h"
#include "cyaml/type/node/node_data.h"
#include "cyaml/type/node/node_impl.h"
#include "cyaml/type/node/convert.h"

#endif // CYAML_H
