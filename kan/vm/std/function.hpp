//
// Created by kvxmmu on 3/8/21.
//

#ifndef KANLANG_FUNCTION_HPP
#define KANLANG_FUNCTION_HPP

#include "type.hpp"
#include "list.hpp"
#include "bytebuffer.hpp"

#include <sstream>

struct __KanFunction {};
static void KanFunction_OnCreateType(Type *function_type);

extern Object *execfunc(const char *buffer, size_t buffer_length, void *meta,
                        const std::vector<std::pair<std::string, Object *>> &args);

struct FunctionObject {
    OBJECT_HEAD

    uint64_t addr = 0;
    uint32_t length = 0;

    call_t c_callback = nullptr;

    std::vector<std::string> signature;
    bool is_multiarg;

    FunctionObject(uint64_t _addr, uint32_t _length,
                   std::vector<std::string> _signature) : type(KanType_GetType<__KanFunction>(KanFunction_OnCreateType)),
                                                       addr(_addr), length(_length), is_multiarg(false),
                                                       signature(std::move(_signature)) {

    }

    explicit FunctionObject(call_t _callback) : c_callback(_callback), type(KanType_GetType<__KanFunction>(KanFunction_OnCreateType)),
                                                is_multiarg(true) {

    }
};

static Object *KanFunction_FromCFunction(call_t c_callback) {
    auto func_object = new FunctionObject(c_callback);

    return reinterpret_cast<Object *>(func_object);
}

static Object *KanFunction_FromAddr(uint64_t addr, uint32_t length,
        const std::vector<std::string> &signature) {
    auto func_object = new FunctionObject(addr, length, signature);

    return reinterpret_cast<Object *>(func_object);
}

static void KanFunction_Free(Object *_self) {
    KanObject_Free<FunctionObject>(_self);
}

static bool KanFunction_IsCFunction(Object *_self) {
    return reinterpret_cast<FunctionObject *>(_self)->c_callback != nullptr;
}

static uint64_t KanFunction_Hash(Object *_self) {
    auto self = reinterpret_cast<FunctionObject *>(_self);
    uint64_t hash;

    if (KanFunction_IsCFunction(_self)) {
        memcpy(&hash, &self->c_callback, sizeof(self->c_callback));
    } else {
        hash = std::hash<uint64_t>()(self->addr) ^ std::hash<uint64_t>()(self->length);
    }

    return hash;
}

static bool KanFunction_GetBool(Object *_self) {
    return true;
}

static Object *KanFunction_Call(Object *_self, Object *args,
        Object *kwargs, void *meta, const char *buffer) {
    auto self = reinterpret_cast<FunctionObject *>(_self);

    if (KanFunction_IsCFunction(_self)) {
        return self->c_callback(_self, args, kwargs, meta, buffer);
    }

    std::vector<std::pair<std::string, Object *>> pargs;
    auto argscount = KanList_Length(args);

    for (size_t pos = 0; pos < argscount; pos++) {
        auto &name = self->signature[pos];
        auto argument = KanList_At(args, pos);

        pargs.emplace_back(name, argument);
    }

    return execfunc(buffer+static_cast<size_t>(self->addr), self->length, meta, pargs);
}

static std::string KanFunction_Repr(Object *_self) {
    std::stringstream stream;

    auto self = reinterpret_cast<FunctionObject *>(_self);

    stream << "<KanFunction at " << self << '>';

    return stream.str();
}

static void KanFunction_OnCreateType(Type *function_type) {
    function_type->boolean = KanFunction_GetBool;
    function_type->hash = KanFunction_Hash;
    function_type->destructor = KanFunction_Free;
    function_type->call = KanFunction_Call;
    function_type->repr = KanFunction_Repr;
}

#endif //KANLANG_FUNCTION_HPP
