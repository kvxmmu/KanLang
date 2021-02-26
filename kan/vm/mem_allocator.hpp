//
// Created by kvxmmu on 2/26/21.
//

#ifndef KANLANG_MEM_ALLOCATOR_HPP
#define KANLANG_MEM_ALLOCATOR_HPP

namespace Kan::Memory {
    template <typename T>
    class Allocator {
    public:
        template <typename T1, typename ...Args>
        T *allocate_and_cast(Args &&...args) {
            return new T1(std::forward<Args&&...>(args)...);
        }
    };
}

#endif //KANLANG_MEM_ALLOCATOR_HPP
