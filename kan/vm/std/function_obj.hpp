//
// Created by kvxmmu on 2/26/21.
//

#ifndef KANLANG_FUNCTION_OBJ_HPP
#define KANLANG_FUNCTION_OBJ_HPP

#include "../type.hpp"

namespace Kan::STD {
    using namespace Kan::VM;

    class FunctionObject : public Object {
    public:
        c_callback_t callback;
        Object *parent;

        FunctionObject(c_callback_t _callback,
                Object *_parent, Type *_type) : Object(_type), callback(_callback), parent(_parent) {

        }
    };
}

#endif //KANLANG_FUNCTION_OBJ_HPP
