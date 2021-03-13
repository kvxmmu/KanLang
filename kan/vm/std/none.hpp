//
// Created by kvxmmu on 3/12/21.
//

#ifndef KANLANG_NONE_HPP
#define KANLANG_NONE_HPP

#include "type.hpp"

struct __KanNone {};
static void KanNone_OnCreateType(Type *none_type);

struct NoneObject {
    OBJECT_HEAD

    NoneObject() : type(KanType_GetType<__KanNone>(KanNone_OnCreateType)) {

    }
};

static bool KanNone_Bool(Object *) {
    return false;
}

static uint64_t KanNone_Hash(Object *) {
    return 0;
}

static void KanNone_Free(Object *self) {
    KanObject_Free<NoneObject>(self);
}

static std::string KanNone_Repr(Object *) {
    return "None";
}

static Object *KanNone_Create() {
    auto none = new NoneObject;

    return reinterpret_cast<Object *>(none);
}

static void KanNone_OnCreateType(Type *none_type) {
    none_type->boolean = KanNone_Bool;
    none_type->destructor = KanNone_Free;
    none_type->hash = KanNone_Hash;
    none_type->repr = KanNone_Repr;
}

#endif //KANLANG_NONE_HPP
