//
// Created by kvxmmu on 2/27/21.
//

#ifndef KANLANG_EASY_HPP
#define KANLANG_EASY_HPP

#include "parser.hpp"
#include "executor/compiler.hpp"

#include "vm/vm.hpp"
#include "vm/std/user_function.hpp"

#include "vm/std/initialiser.hpp"

namespace Kan {
    using namespace Kan::Initializer;

    static void initializer(Kan::Memory::Scope *scope, void *typesptr) {
        auto types = reinterpret_cast<vm_stdtypes_t *>(typesptr);

        scope->set_name("print", create_function_object(&types->func_type, print));
    }

    void run(const std::string &code) {
        Kan::AstTree tree;
        Kan::STD::UserFunctionType ufunc_type;
        Kan::VM::vm_stdtypes_t stdtypes(&ufunc_type);

        stdtypes.initializer = initializer;

        auto tokens = Kan::parse_tokens(code, Kan::Tokenizers::default_all_incl);

        Kan::token_iterator_t it(tokens);

        Kan::parse_tree(Kan::TokenTypes::SKIP, it,
                        &tree);

        Kan::Statements::VectorCompileStream stream;

        Kan::Executor::compile(tree, &stream);
        stream.seek(0);

        Kan::VM::kanvm_execute(&stream, &stdtypes);
    }
}

#endif //KANLANG_EASY_HPP
