//
// Created by kvxmmu on 2/27/21.
//

#ifndef KANLANG_INITIALISER_HPP
#define KANLANG_INITIALISER_HPP

// typo in word initializer, just keep going

#include "string.hpp"
#include "list.hpp"
#include "function.hpp"

namespace Kan::Initializer {
    using namespace Kan::STD;

    static Object *print(Object *args, void *, void *, void *) {
        auto arglist = reinterpret_cast<ListObject *>(args);
        auto str = arglist->at(0);

        std::cout << str->type->repr(str) << std::endl;

        return nullptr;
    }

    static FunctionObject *create_function_object(Type *type, call_t callback) {
        return new FunctionObject(callback, nullptr, type);
    }
}

#endif //KANLANG_INITIALISER_HPP
