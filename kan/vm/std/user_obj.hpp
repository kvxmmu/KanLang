//
// Created by kvxmmu on 2/27/21.
//

#ifndef KANLANG_USER_OBJ_HPP
#define KANLANG_USER_OBJ_HPP

#include "../type.hpp"

namespace Kan::STD {
    using namespace Kan::VM;

    class UserFunctionObject : public Object {
    public:
        uint64_t address;
        uint32_t length;

        std::vector<std::string> signature;

        UserFunctionObject(uint64_t addr, uint32_t _length, Type *_type,
                           std::vector<std::string> sig) : Object(_type), address(addr),
                                                           signature(std::move(sig)), length(_length) {

        }
    };
}

#endif //KANLANG_USER_OBJ_HPP
