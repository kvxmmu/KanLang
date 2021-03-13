//
// Created by kvxmmu on 3/12/21.
//

#ifndef KANLANG_BYTEBUFFER_HPP
#define KANLANG_BYTEBUFFER_HPP

#include "type.hpp"

struct __KanByteBuffer {};
static void KanByteBuffer_OnCreateType(Type *bytebuffer_type);

struct ByteBufferObject {
    OBJECT_HEAD

    char *buffer;
    size_t length;

    bool free_buffer;

    ByteBufferObject(char *_buffer, size_t _length) : buffer(_buffer), free_buffer(false), length(_length),
                    type(KanType_GetType<__KanByteBuffer>(KanByteBuffer_OnCreateType)) {

    }

    explicit ByteBufferObject(size_t _length) : buffer(new char[_length]), free_buffer(true), length(_length),
                                                type(KanType_GetType<__KanByteBuffer>(KanByteBuffer_OnCreateType)) {

    }
};

static uint64_t KanByteBuffer_Hash(Object *_self) {
    auto self = reinterpret_cast<ByteBufferObject *>(_self);

    uint64_t hash = 0;

    memcpy(&hash, &self->buffer, sizeof(char *));

    return std::hash<uint64_t>()(hash);
}

static void KanByteBuffer_Free(Object *_self) {
    auto self = reinterpret_cast<ByteBufferObject *>(_self);

    if (self->free_buffer) {
        delete[] self->buffer;
    }

    KanObject_Free<ByteBufferObject>(_self);
}

static char *KanByteBuffer_GetBuffer(Object *_self) {
    return reinterpret_cast<ByteBufferObject *>(_self)->buffer;
}

static bool KanByteBuffer_GetBool(Object *_self) {
    auto self = reinterpret_cast<ByteBufferObject *>(_self);

    return self->length > 0;
}

static uint64_t KanByteBuffer_Length(Object *_self) {
    return reinterpret_cast<ByteBufferObject *>(_self)->length;
}

static void KanByteBuffer_OnCreateType(Type *bytebuffer_type) {
    bytebuffer_type->destructor = KanByteBuffer_Free;

    bytebuffer_type->boolean = KanByteBuffer_GetBool;
    bytebuffer_type->hash = KanByteBuffer_Hash;
    bytebuffer_type->len = KanByteBuffer_Length;
}

#endif //KANLANG_BYTEBUFFER_HPP
