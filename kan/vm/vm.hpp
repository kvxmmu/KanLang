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

namespace Kan::VM {
    typedef struct {
        Kan::Memory::Scope *global_scope = nullptr;
    } vm_meta_t;

    void runcode(Kan::Statements::CompileStream *stream, vm_meta_t *meta);
}


#endif //KANLANG_VM_HPP
