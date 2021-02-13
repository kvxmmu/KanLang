//
// Created by kvxmmu on 2/13/21.
//

#ifndef KANLANG_EXECUTOR_HPP
#define KANLANG_EXECUTOR_HPP

#include <unordered_map>
#include <variant>

#include "../parser.hpp"
#include "types/types.hpp"

#include "types/integer.hpp"

namespace Kan::Executor {
    int execute(Kan::AstTree &tree);

    typedef Iterator<decltype(AstTree::objects)> ast_iterator_t;
}


#endif //KANLANG_EXECUTOR_HPP
