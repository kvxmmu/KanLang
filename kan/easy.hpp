//
// Created by kvxmmu on 2/27/21.
//

#ifndef KANLANG_EASY_HPP
#define KANLANG_EASY_HPP

#include "parser.hpp"
#include "executor/compiler.hpp"
#include "vm/vm.hpp"

#include "vm/std/function.hpp"
#include "vm/std/int.hpp"

namespace Kan {
    void run(const std::string &code) {
        AstTree tree;

        Kan::Statements::VectorCompileStream stream;
        auto global_scope = new Kan::Memory::Scope;

        auto tokens = Kan::parse_tokens(code, Kan::Tokenizers::default_all_incl);
        token_iterator_t it(tokens);

        Kan::parse_tree(Kan::TokenTypes::SKIP, it, &tree);
        Kan::Executor::compile(tree, &stream);

        stream.seek(0);

        Kan::VM::runcode(&stream, global_scope);
    }
}

#endif //KANLANG_EASY_HPP
