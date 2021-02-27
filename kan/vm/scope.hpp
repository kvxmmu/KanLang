//
// Created by kvxmmu on 2/26/21.
//

#ifndef KANLANG_SCOPE_HPP
#define KANLANG_SCOPE_HPP

#include <unordered_map>
#include "type.hpp"

#include <stdexcept>

namespace Kan::Memory {
    using namespace Kan::VM;

    struct NameNotFound : public std::runtime_error {
        NameNotFound() : std::runtime_error("Name not found in scopes") {

        }
    };


    class Scope {
    public:
        std::unordered_map<std::string, Object *> scope;
        Scope *parent_scope = nullptr;

        bool name_in_scope(const std::string &name) {
            return this->scope.find(name) != this->scope.end();
        }

        void set_name(const std::string &name, Object *object) {
            this->scope[name] = object;
        }

        Object *get_name(const std::string &name) {
            return this->scope[name];
        }

        Object *lookup_name(const std::string &name) {
            if (!this->name_in_scope(name)) {
                if (this->parent_scope == nullptr) {
                    throw NameNotFound();
                }

                return this->parent_scope->lookup_name(name);
            }

            return this->get_name(name);
        }

        void decref_name(const std::string &name, bool remove_from_map = true) {
            auto obj = this->scope.at(name);

            if (obj == nullptr) {
                return;
            }

            if (obj->refs <= 1) {
                obj->deallocate();

                if (remove_from_map) {
                    this->scope.erase(name);
                }
            } else {
                obj->refs--;
            }
        }

        void decref_names() {
            std::vector<name_t> names;

            for (auto &name : this->scope) {
                names.push_back(name.first);
            }

            for (auto &name : names) {
                this->decref_name(name);
            }
        }

        void del_name(const std::string &name) {
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
