//
// Created by kvxmmu on 3/7/21.
//

#include "scope.hpp"
#include <vector>

void Kan::Memory::Scope::set_name(const Kan::Memory::name_t &name, Object *object) {
    if (this->has_name(name)) {
        if (!this->is_same_object(name, object)) {
            this->decref_name(name);

            object->refs++;
        } else {
            return;
        }
    } else {
        object->refs++;
    }

    this->names[name] = object;
}

bool Kan::Memory::Scope::has_name(const Kan::Memory::name_t &name) const {
    return this->names.find(name) != this->names.end();
}

void Kan::Memory::Scope::decref_name(const Kan::Memory::name_t &name) {
    auto object = this->names.at(name);

    if (object->refs <= 1) {
        deallocate_object(object);

        this->names.erase(name);
    } else {
        object->refs--;
    }
}

bool Kan::Memory::Scope::is_same_object(const Kan::Memory::name_t &name, Object *object) const {
    return this->names.at(name) == object;
}

void Kan::Memory::Scope::incref_name(const Kan::Memory::name_t &name) {
    auto object = this->names.at(name);

    object->refs++;
}

void Kan::Memory::Scope::decref_names() {
    std::vector<std::string> names_vec;

    for (auto &name : this->names) {
        names_vec.push_back(name.first);
    }

    for (auto &name : names_vec) {
        this->decref_name(name);
    }
}

Object *Kan::Memory::Scope::get_name(const Kan::Memory::name_t &name) const {
    return this->names.at(name);
}

Object *Kan::Memory::Scope::lookup_name(const Kan::Memory::name_t &name) const {
    if (!this->has_name(name)) {
        if (this->parent_scope == nullptr) {
            return nullptr;
        }

        return this->parent_scope->lookup_name(name);
    }

    return this->get_name(name);
}

////

void Kan::Memory::deallocate_object(Object *object) {
    auto type = object->type;

    if (type->destructor == nullptr) {
        return;
    }

    type->destructor(object);
}