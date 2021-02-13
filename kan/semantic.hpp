//
// Created by kvxmmu on 2/12/21.
//

#ifndef KANLANG_SEMANTIC_HPP
#define KANLANG_SEMANTIC_HPP

#include <memory>

#include "parser.hpp"
#include "containers.hpp"

namespace Kan {
    enum StatementType {
        IF,
        ELSE,
        SET_VARIABLE,
        EXPRESSION,

        MASTER_TYPE
    };

    class StatementObject {
    public:

        virtual void execute() = 0;
        virtual StatementType get_type() { return StatementType::EXPRESSION; }
    };
}

#endif //KANLANG_SEMANTIC_HPP
