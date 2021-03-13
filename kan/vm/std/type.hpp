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
static Type *KanType_GetType(const std::function<void(Type *)> callback = nullptr) {
    auto type = type_allocator.allocate_or_get<T>(callback);

    return type;
}

static bool KanObject_GetBool(Object *object) {
    if (object->type->boolean == nullptr) {
        return false;
    }

    return object->type->boolean(object);
}



#endif //KANLANG_OBJECTTYPE_HPP
