//
// Created by kvxmmu on 3/7/21.
//

#include "type.hpp"

bool KanObject_XDecref(_Object *object) {
    if (object->refs <= 1) {
        return true;
    }

    object->refs--;

    return false;
}

void KanObject_Decref(_Object *object) {
    if (KanObject_XDecref(object)) {
        object->type->destructor(object);
    }
}

void KanObject_Incref(_Object *object) {
    object->refs++;
}