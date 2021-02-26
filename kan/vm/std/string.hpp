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
        static Object *print(Object *args) {
            auto arglist = reinterpret_cast<ListObject *>(args);
            auto self = reinterpret_cast<StringObject *>(arglist->at(0));

            std::cout << self->str << std::endl;
        }

        StringType() {
            this->add_method("print", StringType::print);
        }

        name_t get_type_name() override {
            return typeid(*this).name();
        }

        DEFAULT_FREE_FUNC(StringObject)
        DEFAULT_IS_SAME_FUNC()
    };
}

#endif //KANLANG_STRING_HPP
