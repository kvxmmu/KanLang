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
#include "std/bool.hpp"
#include "std/user_obj.hpp"

#include "../executor/bytecode_def.h"
#include "../executor/statements_class.hpp"
#include "scope.hpp"

namespace Kan::VM {
    using namespace STD;

    typedef void (*initializer_callback_t)(Kan::Memory::Scope *, void *);

    struct vm_stdtypes_t {
        ListType list_type;
        IntType int_type;
        FunctionType func_type;
        StringType string_type;
        BoolType bool_type;
        Type *user_func_type;

        BoolObject *true_object;
        BoolObject *false_object;

        initializer_callback_t initializer;

        Kan::Memory::Scope *global_scope = nullptr;

        vm_stdtypes_t(Type *_user_func_type) : user_func_type(_user_func_type),
                                               true_object(new BoolObject(true, &this->bool_type)),
                                               false_object(new BoolObject(false, &this->bool_type)) {
            this->true_object->xincref();
            this->false_object->xincref();
        }

        ~vm_stdtypes_t() {
            delete this->true_object;
            delete this->false_object;
        }
    };

    struct vm_stack_object_t {
        Object *object = nullptr;
        Object *self = nullptr;

        bool is_literal = false;
        bool dbg_target = false;

        vm_stack_object_t() = default;

        vm_stack_object_t(Object *_object,
                          bool _dbg = false) : object(_object), is_literal(false), dbg_target(_dbg) {

        }

        vm_stack_object_t(const self_pair_t &pair,
                          bool _dbg = false) : object(pair.second), self(pair.first), dbg_target(_dbg) {

        }

        vm_stack_object_t(Object *_object, bool _is_literal,
                bool _dbg = false) : is_literal(_is_literal), object(_object), dbg_target(_dbg) {

        }
    };

    bool is_empty_command(uint8_t command);

    Object *kanvm_execute(Statements::CompileStream *stream,
                          vm_stdtypes_t *types,
                          bool inside_function = false);
}

#endif //KANLANG_VM_HPP
