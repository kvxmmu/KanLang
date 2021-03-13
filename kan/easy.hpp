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
#include "vm/std/bool.hpp"

namespace Kan {
    static Object *print(Object *self, Object *args, Object *kwargs, void *meta, const char *) {
        auto length = KanList_Length(args);

        std::stringstream stream;

        for (size_t pos = 0; pos < length; pos++) {
            auto arg = KanList_At(args, pos);

            stream << arg->type->repr(arg) << ' ';
        }

        auto str = stream.str();
        str.pop_back();

        std::cout << str << std::endl;

        return get_none(meta);
    }

    void std_initialize(Kan::Memory::Scope *scope) {
        scope->set_name("print", KanFunction_FromCFunction(print));

        return;
    }

    void run(const std::string &code) {
        AstTree tree;
        Kan::VM::vm_meta_t meta;

        meta.false_obj = KanBool_FromBool(false);
        meta.true_obj = KanBool_FromBool(true);
        meta.none_obj = KanNone_Create();
        meta.global_scope_callback = std_initialize;

        KanObject_Incref(meta.true_obj);
        KanObject_Incref(meta.false_obj);
        KanObject_Incref(meta.none_obj);

        Kan::Statements::VectorCompileStream stream;

        auto tokens = Kan::parse_tokens(code, Kan::Tokenizers::default_all_incl);
        token_iterator_t it(tokens);

        Kan::parse_tree(Kan::TokenTypes::SKIP, it, &tree);
        Kan::Executor::compile(tree, &stream);

        stream.seek(0);

        Kan::VM::runcode(&stream, &meta);

        KanBool_Free(meta.true_obj);
        KanBool_Free(meta.false_obj);
        KanNone_Free(meta.none_obj);
    }
}

#endif //KANLANG_EASY_HPP
