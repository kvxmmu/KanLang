//
// Created by kvxmmu on 3/12/21.
//

#ifndef KANLANG_BOOL_HPP
#define KANLANG_BOOL_HPP

#include "type.hpp"

struct __KanBool {};
static void KanBool_OnCreateType(Type *bool_type);

struct BoolObject {
    OBJECT_HEAD

    bool value;

    explicit BoolObject(bool _value) : type(KanType_GetType<__KanBool>(KanBool_OnCreateType)), value(_value) {

    }
};

static std::string KanBool_Repr(Object *_self) {
    auto self = reinterpret_cast<BoolObject *>(_self);

    return self->value ? "true" : "false";
}

static Object *KanBool_FromBool(bool boolean) {
    return reinterpret_cast<Object *>(new BoolObject(boolean));
}

static bool KanBool_GetBool(Object *_self) {
    return reinterpret_cast<BoolObject *>(_self)->value;
}

static void KanBool_Free(Object *_self) {
    KanObject_Free<BoolObject>(_self);
}

static uint64_t KanBool_Hash(Object *_self) {
    auto value = KanBool_GetBool(_self);

    return std::hash<bool>()(value);
}

static void KanBool_OnCreateType(Type *bool_type) {
    bool_type->boolean = KanBool_GetBool;
    bool_type->destructor = KanBool_Free;
    bool_type->hash = KanBool_Hash;
}



#endif //KANLANG_BOOL_HPP
