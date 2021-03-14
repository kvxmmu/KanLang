//
// Created by kvxmmu on 3/8/21.
//

#ifndef KANLANG_INT_HPP
#define KANLANG_INT_HPP

#include "../type.hpp"
#include "type.hpp"

#include <sstream>

struct __KanInt {};
static void KanInt_OnCreateInt(Type *int_type);

struct IntObject {
    OBJECT_HEAD

    float number = 0.f;

    IntObject() : type(KanType_GetType<__KanInt>(KanInt_OnCreateInt)) {

    }
};

enum KanIntOperationType {
    PLUS, SUB,
    MUL, DIV,

    LEFT_SHIFT, RIGHT_SHIFT,
    OR, AND, XOR, NOT
};

static std::string KanInt_Repr(Object *self) {
    std::stringstream stream;
    auto int_object = reinterpret_cast<IntObject *>(self);

    stream << int_object->number;

    return stream.str();
}

static Object *KanInt_FromFloat(float number) {
    auto integer = new IntObject;
    integer->number = number;

    return reinterpret_cast<Object *>(integer);
}

static Object *KanInt_FromInt(int number) {
    auto integer = new IntObject;
    integer->number = static_cast<float>(number);

    return reinterpret_cast<Object *>(integer);
}

static Object *KanInt_PerformBinOperation(Object *_left, Object *_right, KanIntOperationType op) {
    auto left = reinterpret_cast<IntObject *>(_left);
    auto right = reinterpret_cast<IntObject *>(_right);

    auto lefti = static_cast<int32_t>(left->number);
    auto righti = static_cast<int32_t>(right->number);

    float response;

    switch (op) {
        case PLUS:
            response = left->number + right->number;

            break;

        case SUB:
            response = left->number - right->number;

            break;

        case DIV:
            response = left->number / right->number;

            break;

        case MUL:
            response = left->number * right->number;

            break;

        case LEFT_SHIFT:
            response = static_cast<float>(lefti << righti);

            break;

        case RIGHT_SHIFT:
            response = static_cast<float>(lefti >> righti);

            break;

        case OR:
            response = static_cast<float>(lefti | righti);

            break;

        case AND:
            response = static_cast<float>(lefti & righti);

            break;

        case XOR:
            response = static_cast<float>(lefti ^ righti);

            break;
    }

    return KanInt_FromFloat(response);
}

static Object *KanInt_PerformUnaryOperation(Object *_self, KanIntOperationType type) {
    auto self = reinterpret_cast<IntObject *>(_self);
    auto selfi = static_cast<int32_t>(self->number);

    switch (type) {
        case SUB:
            return KanInt_FromFloat(-self->number);

        case PLUS:
            return KanInt_FromFloat(+self->number);

        case NOT:
            return KanInt_FromInt(~selfi);
    }

    return nullptr;
}

static Object *KanInt_Add(Object *_left, Object *_right) {
    return KanInt_PerformBinOperation(_left, _right, PLUS);
}

static Object *KanInt_Sub(Object *_left, Object *_right) {
    return KanInt_PerformBinOperation(_left, _right, SUB);
}

static Object *KanInt_Div(Object *_left, Object *_right) {
    return KanInt_PerformBinOperation(_left, _right, DIV);
}

static Object *KanInt_Mul(Object *_left, Object *_right) {
    return KanInt_PerformBinOperation(_left, _right, MUL);
}

static void KanInt_Free(Object *object) {
    KanObject_Free<IntObject>(object);
}

static uint64_t KanInt_Hash(Object *_self) {
    auto self = reinterpret_cast<IntObject *>(_self);

    return std::hash<float>()(self->number);
}

static Object *KanInt_LeftShift(Object *_self, Object *_operand) {
    return KanInt_PerformBinOperation(_self, _operand, LEFT_SHIFT);
}

static Object *KanInt_RightShift(Object *_self, Object *_operand) {
    return KanInt_PerformBinOperation(_self, _operand, RIGHT_SHIFT);
}

static Object *KanInt_Or(Object *_self, Object *_operand) {
    return KanInt_PerformBinOperation(_self, _operand, OR);
}

static Object *KanInt_Xor(Object *_self, Object *_operand) {
    return KanInt_PerformBinOperation(_self, _operand, XOR);
}

static Object *KanInt_And(Object *_self, Object *_operand) {
    return KanInt_PerformBinOperation(_self, _operand, AND);
}

static Object *KanInt_Neg(Object *_self) {
    return KanInt_PerformUnaryOperation(_self, SUB);
}

static Object *KanInt_Pos(Object *_self) {
    return KanInt_PerformUnaryOperation(_self, PLUS);
}

static Object *KanInt_Not(Object *_self) {
    return KanInt_PerformUnaryOperation(_self, NOT);
}

static void KanInt_OnCreateInt(Type *int_type) {
    int_type->repr = KanInt_Repr;
    int_type->hash = KanInt_Hash;

    int_type->mul = KanInt_Mul;
    int_type->div = KanInt_Div;

    int_type->add = KanInt_Add;
    int_type->sub = KanInt_Sub;

    int_type->neg = KanInt_Neg;
    int_type->pos = KanInt_Pos;

    int_type->left_shift = KanInt_LeftShift;
    int_type->right_shift = KanInt_RightShift;
    int_type->bin_or = KanInt_Or;
    int_type->bin_xor = KanInt_Xor;
    int_type->bin_and = KanInt_And;
    int_type->bin_not = KanInt_Not;

    int_type->destructor = KanInt_Free;
}


#endif //KANLANG_INT_HPP
