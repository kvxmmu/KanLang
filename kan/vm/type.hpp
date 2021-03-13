//
// Created by kvxmmu on 3/7/21.
//

#ifndef KANLANG_TYPE_HPP
#define KANLANG_TYPE_HPP

#include <cinttypes>
#include <map>
#include <string>

#define OBJECT_HEAD uint32_t refs = 0; Type *type;

struct _Object;
struct Type;

typedef _Object *(*constructor_t)(Type *, _Object *args, _Object *kwargs);
typedef uint64_t(*hash_t)(_Object *self);
typedef void (*destructor_t)(_Object *self);
typedef _Object *(*function_t)(_Object *args, _Object *kwargs);
typedef _Object *(*binop_t)(_Object *left, _Object *right);
typedef bool (*bool_t)(_Object *self);
typedef _Object *(*call_t)(_Object *self, _Object *args, _Object *kwargs, void *meta,
        const char *buffer);
typedef uint64_t (*len_t)(_Object *self);

typedef std::string (*repr_t)(_Object *self);

struct Type {
    std::map<std::string, function_t> methods;

    constructor_t constructor = nullptr;
    hash_t hash = nullptr;
    destructor_t destructor = nullptr;
    repr_t repr = nullptr;

    binop_t add = nullptr;
    binop_t sub = nullptr;

    binop_t mul = nullptr;
    binop_t div = nullptr;

    call_t call = nullptr;
    len_t len = nullptr;

    bool_t boolean = nullptr;
};

struct _Object {
    OBJECT_HEAD
};

bool KanObject_XDecref(_Object *object);
void KanObject_Decref(_Object *object);
void KanObject_Incref(_Object *object);

template <typename T>
static void KanObject_Free(_Object *object) {
    delete reinterpret_cast<T *>(object);
}

typedef _Object Object;


#endif //KANLANG_TYPE_HPP
