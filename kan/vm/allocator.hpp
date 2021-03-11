//
// Created by kvxmmu on 3/8/21.
//

#ifndef KANLANG_ALLOCATOR_HPP
#define KANLANG_ALLOCATOR_HPP

#include "smart_allocator.hpp"

static auto *_type_allocator = new TypeAllocator;
static auto &type_allocator = *_type_allocator;

#endif //KANLANG_ALLOCATOR_HPP
