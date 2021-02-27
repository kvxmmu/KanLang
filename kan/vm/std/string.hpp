//
// Created by kvxmmu on 2/26/21.
//

#ifndef KANLANG_STRING_HPP
#define KANLANG_STRING_HPP

#include "../type.hpp"
#include "list.hpp"

#include <string>
#include <iostream>

namespace Kan::STD {
    using namespace Kan::VM;

    class StringObject : public Object {
    public:
        std::string str;

        StringObject(std::string _str, Type *_type) : Object(_type), str(std::move(_str)) {

        }
    };

    class StringType : public Type {
    public:
        bool get_bool(Object *object) override {
            auto str = reinterpret_cast<StringObject *>(object);

            return !str->str.empty();
        }

        std::string repr(Object *self) override {
            return reinterpret_cast<StringObject *>(self)->str;
        }

        bool is_equals(Object *self, Object *value) override {
            if (!value->type->is_same_type(this)) {
                return false;
            }

            auto self_str = reinterpret_cast<StringObject *>(self);
            auto value_str = reinterpret_cast<StringObject *>(value);

            return self_str->str == value_str->str;
        }

        name_t get_type_name() override {
            return CLASSNAME;
        }

        DEFAULT_FREE_FUNC(StringObject)
        DEFAULT_IS_SAME_FUNC()
    };
}

#endif //KANLANG_STRING_HPP
