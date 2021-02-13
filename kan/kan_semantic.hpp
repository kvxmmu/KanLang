//
// Created by kvxmmu on 2/13/21.
//

#ifndef KANLANG_KAN_SEMANTIC_HPP
#define KANLANG_KAN_SEMANTIC_HPP

#include "semantic.hpp"

#include <utility>

#define IF_STMT 0u
#define SET_NAME_STMT 1u

namespace Kan::Statements {
    class MainApplication : public StatementObject {
    public:
        void execute() override {

        }

        StatementType get_type() override {
            return StatementType::MASTER_TYPE;
        }
    };
}


#endif //KANLANG_KAN_SEMANTIC_HPP
