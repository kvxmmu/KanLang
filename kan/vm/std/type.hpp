//
// Created by kvxmmu on 3/8/21.
//

#ifndef KANLANG_TYPEOBJECT_HPP
#define KANLANG_TYPEOBJECT_HPP

#include "../type.hpp"
#include "../allocator.hpp"

static std::string _typerepr(Object *self) {
    return "TypeObject";
}

template <typename T>
Type *KanType_GetType(const std::function<void(Type *)> callback = nullptr) {
    auto type = type_allocator.allocate_or_get<T>(callback);

    type->repr = _typerepr;

    return type;
}



#endif //KANLANG_OBJECTTYPE_HPP
