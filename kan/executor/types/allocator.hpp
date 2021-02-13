//
// Created by kvxmmu on 2/13/21.
//

#ifndef KANLANG_ALLOCATOR_HPP
#define KANLANG_ALLOCATOR_HPP

#include <unordered_map>
#include <type_traits>

#include "types.hpp"

namespace Kan::Memory {
    typedef void* allocator_val_t;

    /*
     * Allocator struct ll'be smth like this:
     *  - types
     *  - classes
     *
     * There are different memory allocators for all internal data,
     * TODO: check for performance / memory usage issues
     * Alpha
     */

    class InternalAllocator {
    public:
        std::unordered_map<std::string_view, void*> mem;

        template <typename T, typename CastType = void>
        CastType *allocate_or_get() {
            auto label = typeid(T).name();

            this->allocate_if_not_exists<T>(label);

            if constexpr (std::is_same<CastType *, void*>::value) {
                return this->mem.at(label);
            }

            return reinterpret_cast<CastType *>(this->mem.at(label));
        }

        template <typename T>
        void allocate_if_not_exists() {
            auto label = typeid(T).name();

            if (this->mem.find(label) == this->mem.end()) {
                allocator_val_t vt = new T;

                this->mem[label] = vt;
            }
        }
    };
}

#endif //KANLANG_ALLOCATOR_HPP
