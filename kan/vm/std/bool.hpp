//
// Created by kvxmmu on 2/27/21.
//

#ifndef KANLANG_BOOL_HPP
#define KANLANG_BOOL_HPP

#include "../type.hpp"

namespace Kan::STD {
    using namespace Kan::VM;

    class BoolObject : public Object {
    public:
        bool value;

        BoolObject(bool _value, Type *_type) : value(_value), Object(_type) {

        }
    };

    class BoolType : public Type {
    public:
        bool is_equals(Object *self, Object *value) override {
            if (value == nullptr || !self->type->is_same_type(value->type)) {
                return false;
            }

            return reinterpret_cast<BoolObject *>(value)->value;
        }

        name_t get_type_name() override {
            return CLASSNAME;
        }

        bool get_bool(Object *self) override {
            return reinterpret_cast<BoolObject *>(self)->value;
        }

        DEFAULT_IS_SAME_FUNC()
        DEFAULT_FREE_FUNC(BoolObject)
    };


}

#endif //KANLANG_BOOL_HPP
