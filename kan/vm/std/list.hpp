//
// Created by kvxmmu on 3/12/21.
//

#ifndef KANLANG_LIST_HPP
#define KANLANG_LIST_HPP

#include "type.hpp"
#include <vector>

struct __KanList {};
static void KanList_OnCreateType(Type *list_type);

struct ListObject {
    OBJECT_HEAD

    std::vector<Object *> objects;

    ListObject() : type(KanType_GetType<__KanList>(KanList_OnCreateType)) {

    }
};

static Object *KanList_At(Object *_self, size_t index) {
    auto self = reinterpret_cast<ListObject *>(_self);

    return self->objects.at(index);
}

static Object *KanList_Create() {
    auto list = new ListObject;

    return reinterpret_cast<Object *>(list);
}

static void KanList_Free(Object *self) {
    auto list = reinterpret_cast<ListObject *>(self);

    for (auto object : list->objects) {
        KanObject_Decref(object);
    }

    KanObject_Free<ListObject>(self);
}

static uint64_t KanList_Length(Object *self) {
    return reinterpret_cast<ListObject *>(self)->objects.size();
}

static bool KanList_GetBool(Object *self) {
    return KanList_Length(self) > 0;
}

static uint64_t KanList_Hash(Object *self) {
    uint64_t ptr = 0;

    memcpy(&ptr, &self, sizeof(Object *));

    return ptr;
}

static void KanList_Append(Object *_self, Object *item) {
    auto self = reinterpret_cast<ListObject *>(_self);
    KanObject_Incref(item);

    self->objects.push_back(item);
}

static void KanList_Insert(Object *_self, Object *item, size_t index) {
    auto self = reinterpret_cast<ListObject *>(_self);

    self->objects.insert(self->objects.begin()+index, item);
}

static void KanList_Pop(Object *_self, size_t index) {
    auto self = reinterpret_cast<ListObject *>(_self);

    self->objects.erase(self->objects.begin()+index);
}

static void KanList_OnCreateType(Type *list_type) {
    list_type->destructor = KanList_Free;
    list_type->len = KanList_Length;
    list_type->boolean = KanList_GetBool;
    list_type->hash = KanList_Hash;

}

#endif //KANLANG_LIST_HPP
