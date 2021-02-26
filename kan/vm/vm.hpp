//
// Created by kvxmmu on 2/25/21.
//

#ifndef KANLANG_VM_HPP
#define KANLANG_VM_HPP

#include <stack>
#include <list>
#include <unordered_map>

#include "std/list.hpp"
#include "std/function.hpp"
#include "std/string.hpp"

#include "../executor/bytecode_def.h"
#include "../executor/statements_class.hpp"
#include "scope.hpp"

namespace Kan::VM {
    struct vm_stack_object_t {
        Object *object = nullptr;
        Object *self = nullptr;

        bool is_literal = false;

        vm_stack_object_t() = default;

        vm_stack_object_t(Object *_object) : object(_object), is_literal(false) {

        }

        vm_stack_object_t(const self_pair_t &pair) : object(pair.second), self(pair.first) {

        }

        vm_stack_object_t(Object *_object, bool _is_literal) : is_literal(_is_literal), object(_object) {

        }
    };

    bool is_empty_command(uint8_t command);

    void kanvm_execute(Statements::CompileStream *stream);
}

#endif //KANLANG_VM_HPP
