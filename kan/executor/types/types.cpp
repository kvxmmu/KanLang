//
// Created by kvxmmu on 2/18/21.
//

#include "types.hpp"

void Kan::Types::Scope::decref(Kan::Types::name_t name) {
    auto scope_value = this->values.at(name);

    if (scope_value.type == ScopeValueType::SCOPE) {
        auto scope = reinterpret_cast<Kan::Types::Scope *>(scope_value.ptr);

        bool need_free = scope->xdecref();

        if (need_free) {
            scope->destroy_scope();
            delete scope;

            this->values.erase(name);
        }

    } else {
        auto object = reinterpret_cast<Kan::Types::Object *>(scope_value.ptr);
        bool need_free = object->xdecref();

        if (need_free) {
            object->type->free(object);
            delete object;

            this->values.erase(name);
        }
    }
}

void Kan::Types::Scope::xincref(Kan::Types::name_t name) {
    auto scope_value = this->values.at(name);

    if (scope_value.type == ScopeValueType::SCOPE) {
        reinterpret_cast<Kan::Types::Scope *>(scope_value.ptr)->xincref();
    } else {
        reinterpret_cast<Kan::Types::Object *>(scope_value.ptr)->xincref();
    }
}

void Kan::Types::Scope::destroy_scope() {
    for (auto &value_pair : this->values) {
        scope_value_t value = value_pair.second;

        if (value.type == ScopeValueType::SCOPE) {
            reinterpret_cast<Kan::Types::Scope *>(value.ptr)->destroy_scope();
        } else {
            auto object = reinterpret_cast<Kan::Types::Object *>(value.ptr);
            bool need_free = object->xdecref();

            if (need_free) {
                object->type->free(object);
            }
        }
    }
}

Kan::Types::Scope *Kan::Types::Scope::create_scope(name_t linked_name) {
    auto scope = new Scope();

    this->values[linked_name] = scope_value_t(scope);

    return scope;
}

Kan::Types::Object *Kan::Types::Scope::create_object(Kan::Types::name_t name,
        Kan::Types::Object *object) {
    this->values[name] = scope_value_t(object);

    return object;
}
