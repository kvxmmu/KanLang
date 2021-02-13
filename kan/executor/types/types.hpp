//
// Created by kvxmmu on 2/13/21.
//

#ifndef KANLANG_TYPES_HPP
#define KANLANG_TYPES_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <type_traits>
#include <memory>
#include <functional>

#include "allocator.hpp"

namespace Kan::Types {
    class Scope;
    class Object;

    typedef std::string name_t;
    typedef uint16_t refcounter_int_t;
    typedef std::pair<Object *, Object *> objects_pair;
    typedef std::function<Object *(const objects_pair &)> service_method_call_t;

    typedef Kan::Memory::InternalAllocator allocator_t;

    class ServiceMethod {
    public:
        service_method_call_t call = nullptr;
        std::string_view method_name;

        ServiceMethod() = default;

        explicit ServiceMethod(service_method_call_t _call,
                                std::string_view _method_name) : call(_call), method_name(_method_name) {

        }
    };

    static ServiceMethod empty_method;

    class Type {
    public:
        Type() = default;

        std::vector<ServiceMethod> service_methods;

        allocator_t *allocator;

        Type(allocator_t *_allocator) : allocator(_allocator) {}

        virtual name_t get_type_name() { return "KanObject"; };
        virtual void init_type() = 0;

        virtual size_t hash(void *ptr) = 0;
        virtual bool free(void *ptr) = 0;

        ServiceMethod &get_service_method(std::string_view name) {
            for (auto &method : this->service_methods) {
                if (method.method_name == name) {
                    return method;
                }
            }

            return empty_method;
        }

    };

    class Object {
    public:
        refcounter_int_t refcounter = 1u;

        Type *type = nullptr;

        // Object pointer is optional because all external objects
        // should implement memory management on its own

        Object() = default;

        explicit Object(Type *_type) : type(_type) {

        }

        constexpr void n_incref() {
            this->refcounter++;
        }

        constexpr void n_decref() {
            this->refcounter--;
        }

        [[nodiscard]] constexpr bool is_decref_end() const {
            return this->refcounter == 1u;
        }
    };

    struct scope_value_t {
        union {
            Scope *scope = nullptr;
            Object *object;
        };

        uint8_t type = 0u;

        explicit scope_value_t(Scope *_scope) : scope(_scope) {

        }

        explicit scope_value_t(Object *_object) : object(_object), type(1u) {

        }
    };

    class Scope {
    public:
        std::unordered_map<name_t, scope_value_t> objects;

        Scope *upper_scope = nullptr; // nullptr - master scope
    };
}


#endif //KANLANG_INTEGER_HPP
