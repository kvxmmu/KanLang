//
// Created by kvxmmu on 3/8/21.
//

#ifndef KANLANG_INT_HPP
#define KANLANG_INT_HPP

#include "../type.hpp"
#include "type.hpp"

struct __KanInt {};
static void KanInt_OnCreateInt(Type *int_type);

struct IntObject {
    OBJECT_HEAD

    float number = 0.f;

    IntObject() : type(KanType_GetType<__KanInt>(KanInt_OnCreateInt)) {

    }
};

namespace {
    enum KanIntOperationType {
        PLUS, SUB,
        MUL, DIV
    };

    std::string KanInt_Repr(Object *self) {
        auto int_object = reinterpret_cast<IntObject *>(self);

        return std::to_string(int_object->number);
    }

    Object *KanInt_FromFloat(float number) {
        auto integer = new IntObject;
        integer->number = number;

        return reinterpret_cast<Object *>(integer);
    }

    Object *KanInt_FromInt(int number) {
        auto integer = new IntObject;
        integer->number = static_cast<float>(number);

        return reinterpret_cast<Object *>(integer);
    }

    Object *KanInt_PerformBinOperation(Object *_left, Object *_right, KanIntOperationType op) {
        auto left = reinterpret_cast<IntObject *>(_left);
        auto right = reinterpret_cast<IntObject *>(_right);

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
        }

        return KanInt_FromFloat(response);
    }

    Object *KanInt_Add(Object *_left, Object *_right) {
        return KanInt_PerformBinOperation(_left, _right, PLUS);
    }

    Object *KanInt_Sub(Object *_left, Object *_right) {
        return KanInt_PerformBinOperation(_left, _right, SUB);
    }

    Object *KanInt_Div(Object *_left, Object *_right) {
        return KanInt_PerformBinOperation(_left, _right, DIV);
    }

    Object *KanInt_Mul(Object *_left, Object *_right) {
        return KanInt_PerformBinOperation(_left, _right, MUL);
    }

    void KanInt_Free(Object *object) {
        KanObject_Free<IntObject>(object);
    }

}

static void KanInt_OnCreateInt(Type *int_type) {
    int_type->repr = KanInt_Repr;

    int_type->mul = KanInt_Mul;
    int_type->div = KanInt_Div;

    int_type->add = KanInt_Add;
    int_type->sub = KanInt_Sub;

    int_type->destructor = KanInt_Free;
}


#endif //KANLANG_INT_HPP
