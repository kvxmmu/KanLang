//
// Created by kvxmmu on 3/12/21.
//

#ifndef KANLANG_STRING_HPP
#define KANLANG_STRING_HPP

#include "type.hpp"

struct __KanString {};
static void KanString_OnCreateType(Type *string_type);

struct StringObject {
    OBJECT_HEAD

    std::string string;

    explicit StringObject(std::string _string) : string(std::move(_string)),
                                        type(KanType_GetType<__KanString>(KanString_OnCreateType)) {

    }
};

static Object *KanString_FromString(const std::string &string) {
    return reinterpret_cast<Object *>(new StringObject(string));
}

static void KanString_Free(Object *_self) {
    auto self = reinterpret_cast<StringObject *>(_self);

    self->string.clear();

    KanObject_Free<StringObject>(_self);
}

static uint64_t KanString_Length(Object *_self) {
    auto self = reinterpret_cast<StringObject *>(_self);

    return self->string.size();
}

static bool KanString_GetBool(Object *_self) {
    return KanString_Length(_self) > 0;
}

static uint64_t KanString_Hash(Object *_self) {
    return std::hash<std::string>()(reinterpret_cast<StringObject *>(_self)->string);
}

static std::string KanString_Repr(Object *_self) {
    return reinterpret_cast<StringObject *>(_self)->string;
}

static void KanString_OnCreateType(Type *string_type) {
    string_type->destructor = KanString_Free;
    string_type->len = KanString_Length;
    string_type->boolean = KanString_GetBool;
    string_type->hash = KanString_Hash;
    string_type->repr = KanString_Repr;
}

#endif //KANLANG_STRING_HPP
