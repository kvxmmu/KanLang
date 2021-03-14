//
// Created by kvxmmu on 3/7/21.
//

#ifndef KANLANG_TYPE_HPP
#define KANLANG_TYPE_HPP

#include <cinttypes>
#include <map>
#include <string>

#include <sstream>

#define OBJECT_HEAD uint32_t refs = 0; Type *type;

struct _Object;
struct Type;

typedef void (*constructor_t)(Type *, _Object *args, _Object *kwargs);
typedef uint64_t(*hash_t)(_Object *self);
typedef void (*destructor_t)(_Object *self);
typedef _Object *(*function_t)(_Object *args, _Object *kwargs);
typedef _Object *(*binop_t)(_Object *left, _Object *right);
typedef bool (*bool_t)(_Object *self);
typedef _Object *(*call_t)(_Object *self, _Object *args, _Object *kwargs, void *meta,
        const char *buffer);
typedef uint64_t (*len_t)(_Object *self);

typedef _Object *(*left_shift_t)(_Object *self, _Object *operand);
typedef _Object *(*right_shift_t)(_Object *self, _Object *operand);

typedef _Object *(*bin_or_t)(_Object *self, _Object *operand);
typedef _Object *(*bin_and_t)(_Object *self, _Object *operand);
typedef _Object *(*bin_xor_t)(_Object *self, _Object *operand);

typedef _Object *(*neg_t)(_Object *self);
typedef _Object *(*pos_t)(_Object *self);
typedef _Object *(*bin_not_t)(_Object *self);

typedef _Object *(*getattr_t)(_Object *self, const std::string &);
typedef _Object *(*setattr_t)(_Object *self, const std::string &, _Object *attr);
typedef std::string (*repr_t)(_Object *self);

bool KanObject_XDecref(_Object *object);
void KanObject_Decref(_Object *object);
void KanObject_Incref(_Object *object);

static std::string KanType_DefaultRepr(_Object *self) {
    std::stringstream stream;

    stream << "<KanObject at " << self << '>';

    return stream.str();
}

static uint64_t KanType_DefaultHash(_Object *self) {
    return std::hash<void *>()(self);
}

static void KanType_DefaultConstructor(Type *, _Object *, _Object *) {}
static uint64_t KanType_DefaultLen(_Object *self) { return 0; }
static _Object *KanType_DefaultPos(_Object *self) { return self; }
static bool KanType_GetBool(_Object *self) { return true; }

struct Type {
    std::map<std::string, function_t> methods;

    constructor_t constructor = KanType_DefaultConstructor;
    hash_t hash = KanType_DefaultHash;
    destructor_t destructor = nullptr;
    repr_t repr = KanType_DefaultRepr;

    binop_t add = nullptr;
    binop_t sub = nullptr;

    binop_t mul = nullptr;
    binop_t div = nullptr;

    call_t call = nullptr;
    len_t len = KanType_DefaultLen;

    left_shift_t left_shift = nullptr;
    right_shift_t right_shift = nullptr;

    bin_or_t bin_or = nullptr;
    bin_and_t bin_and = nullptr;
    bin_xor_t bin_xor = nullptr;

    neg_t neg = nullptr;
    pos_t pos = KanType_DefaultPos;
    bin_not_t bin_not = nullptr;

    getattr_t getattr = nullptr;

    bool_t boolean = KanType_GetBool;
};

struct _Object {
    OBJECT_HEAD
};


template <typename T>
static void KanObject_Free(_Object *object) {
    delete reinterpret_cast<T *>(object);
}

typedef _Object Object;


#endif //KANLANG_TYPE_HPP
