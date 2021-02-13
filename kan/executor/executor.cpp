//
// Created by kvxmmu on 2/13/21.
//

#include "executor.hpp"

#include <iostream>

int Kan::Executor::execute(Kan::AstTree &tree) {
    Kan::Executor::ast_iterator_t it(tree.objects);

    std::cout << sizeof(Kan::Types::Type) << std::endl;

    return 0;
}
