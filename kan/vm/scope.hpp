//
// Created by kvxmmu on 2/26/21.
//

#ifndef KANLANG_SCOPE_HPP
#define KANLANG_SCOPE_HPP

#include <unordered_map>
#include "type.hpp"

namespace Kan::Memory {
    using namespace Kan::VM;

    class Scope {
    public:
        std::unordered_map<name_t, Object *> scope;
        Scope *parent_scope = nullptr;

        bool name_in_scope(name_t name) {
            return this->scope.find(name) != this->scope.end();
        }

        void set_name(name_t name, Object *object) {
            this->scope[name] = object;
        }

        Object *get_name(name_t name) {
            return this->scope[name];
        }

        void decref_name(name_t name) {
            auto obj = this->scope.at(name);

            if (obj->refs == 1) {
                obj->deallocate();

                this->scope.erase(name);
            } else {
                obj->refs--;
            }
        }

        void decref_names() {
            for (auto &name : this->scope) {
                this->decref_name(name.first);
            }
        }

        void del_name(name_t name) {
            auto obj = this->scope.at(name);

            delete obj;

            this->scope.erase(name);
        }

        void del_names() {
            for (auto &name : this->scope) {
                this->del_name(name.first);
            }
        }
    };
}

#endif //KANLANG_SCOPE_HPP
