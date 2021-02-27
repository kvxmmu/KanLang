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
        int value;

        explicit IntObject(Type *_type,
                size_t _value = 0) : Object(_type), value(_value) {}
    };

    class IntType : public Type {
    public:
        static int get_result(uint8_t op_type, IntObject *left,
                IntObject *right) {
            switch (op_type) {
                case 0: // plus
                    return left->value + right->value;

                case 1: // minus
                    return left->value - right->value;

                case 2: // mul
                    return left->value * right->value;

                case 3: // div
                    return left->value / right->value;

                default:
                    return 0;

            }
        }

        static Object *_add(Object *self, Object *right) {
            return new IntObject(self->type, IntType::get_result(0, reinterpret_cast<IntObject *>(self),
                                                                 reinterpret_cast<IntObject *>(right)));
        }

        static Object *_sub(Object *self, Object *right) {
            return new IntObject(self->type, IntType::get_result(1, reinterpret_cast<IntObject *>(self),
                                                                 reinterpret_cast<IntObject *>(right)));
        }

        static Object *_mul(Object *self, Object *right) {
            return new IntObject(self->type, IntType::get_result(2, reinterpret_cast<IntObject *>(self),
                                                                 reinterpret_cast<IntObject *>(right)));
        }

        IntType() {
            this->add = IntType::_add;
            this->mul = IntType::_mul;
            this->sub = IntType::_sub;
        }

        std::string repr(Object *self) override {
            auto int_ob = reinterpret_cast<IntObject *>(self);

            return std::to_string(int_ob->value);
        }

        name_t get_type_name() override {
            return CLASSNAME;
        }

        DEFAULT_FREE_FUNC(IntObject)
        DEFAULT_IS_SAME_FUNC()
    };
}

#endif //KANLANG_INT_HPP
