//
// Created by kvxmmu on 3/8/21.
//

#ifndef KANLANG_SMART_ALLOCATOR_HPP
#define KANLANG_SMART_ALLOCATOR_HPP

#include <unordered_map>
#include <functional>

#include "type.hpp"

class TypeAllocator {
public:
    std::unordered_map<std::string, Type *> types;

    template <typename T = Type>
    Type *allocate() {
        auto new_type = new Type;

        this->types[typeid(T).name()] = new_type;

        return new_type;
    }

    template <typename T>
    Type *get() {
        return this->types[typeid(T).name()];
    }

    template <typename T>
    bool is_type_exists() {
        return this->types.find(typeid(T).name()) != this->types.end();
    }

    template <typename T>
    Type *allocate_or_get(const std::function<void(Type *)>& on_create_handler = nullptr) {
        if (this->is_type_exists<T>()) {
            return this->get<T>();
        }

        auto type = this->allocate<T>();

        if (on_create_handler != nullptr) {
            on_create_handler(type);
        }

        return type;
    }

    void deallocate_all() {
        for (auto &type : types) {
            delete type.second;
        }

        this->types.clear();
    }
};

#endif //KANLANG_SMART_ALLOCATOR_HPP
