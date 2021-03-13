//
// Created by kvxmmu on 3/7/21.
//

#ifndef KANLANG_VM_HPP
#define KANLANG_VM_HPP

#include "type.hpp"
#include "scope.hpp"
#include "../executor/statements_class.hpp"

#include "allocator.hpp"

#include <deque>
#include <stack>

#include "std/int.hpp"
#include "std/bool.hpp"
#include "std/none.hpp"
#include "std/function.hpp"
#include "std/string.hpp"

namespace Kan::VM {
    typedef struct {
        Kan::Memory::Scope *global_scope = nullptr;

        Object *none_obj = nullptr;
        Object *true_obj = nullptr;
        Object *false_obj = nullptr;

        void (*global_scope_callback)(Kan::Memory::Scope *);
    } vm_meta_t;

    Object *runcode(Kan::Statements::CompileStream *stream, vm_meta_t *meta,
            bool is_function = false, const std::vector<std::pair<std::string, Object *>> *argnames = nullptr);
}

extern Object *execfunc(const char *buffer, size_t buffer_length, void *meta,
                        const std::vector<std::pair<std::string, Object *>> &args);

extern Object *get_true(void *ptr);
extern Object *get_false(void *ptr);
extern Object *get_none(void *ptr);


#endif //KANLANG_VM_HPP
