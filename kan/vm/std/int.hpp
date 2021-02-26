//
// Created by kvxmmu on 2/26/21.
//

#ifndef KANLANG_INT_HPP
#define KANLANG_INT_HPP

#include "../type.hpp"

#include <iostream>

namespace Kan::STD {
    using namespace Kan::VM;

    class IntObject : public Object {
    public:
        size_t value;

        explicit IntObject(Type *_type,
                size_t _value = 0) : Object(_type), value(_value) {}
    };

    class IntType : public Type {
    public:
        static Object *_add(Object *self, Object *right) {

            return new IntObject(self->type, reinterpret_cast<IntObject *>(self)->value+
                                 reinterpret_cast<IntObject *>(right)->value);
        }

        IntType() {
            this->add = IntType::_add;
        }

        name_t get_type_name() override {
            return CLASSNAME;
        }

        DEFAULT_FREE_FUNC(IntObject)
        DEFAULT_IS_SAME_FUNC()
    };
}

#endif //KANLANG_INT_HPP
