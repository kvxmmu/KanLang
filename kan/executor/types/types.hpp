//
// Created by kvxmmu on 2/18/21.
//

#ifndef KANLANG_TYPES_HPP
#define KANLANG_TYPES_HPP

#include <unordered_map>
#include <string_view>
#include <vector>

#include <stdexcept>
#include <iostream>

namespace Kan::Types {
    class Object;
    class Scope;

    typedef std::vector<Object *> objects_t;
    typedef Object* (*c_callback_t)(Object *, const objects_t &);
    typedef std::string_view name_t;

    enum ScopeValueType {
        SCOPE,
        OBJECT
    };

    struct scope_value_t {
        ScopeValueType type;

        void *ptr;

        scope_value_t() = default;

        explicit scope_value_t(Object *obj) : ptr(obj), type(ScopeValueType::OBJECT) {

        }

        explicit scope_value_t(Scope *scope) : ptr(scope), type(ScopeValueType::SCOPE) {

        }
    };

    struct MethodNotFound : public std::runtime_error {
        MethodNotFound() : std::runtime_error("Method not found") {

        }
    };

    struct NameError : public std::runtime_error {
        NameError() : std::runtime_error("Name not found") {

        }
    };

    class Function {
    public:
        c_callback_t callback = nullptr;

        name_t name;
        size_t cp = 0; // codepointer is an abstract value depending on code block.
                       // alternatively explained: codepointer is an local address depending on execution block;

        Function(c_callback_t &&_callback,
                name_t _name) : callback(_callback), name(_name) {

        }

        Function(size_t _cp, name_t _name) : cp(_cp), name(_name) {

        }

        [[nodiscard]] constexpr bool is_c_function() const {
            return this->callback != nullptr;
        }
    };

    class Type {
    public:
        std::vector<Function> methods;

        bool is_method_exists(name_t name) {
            for (auto &method : this->methods) {
                if (method.name == name) {
                    return true;
                }
            }

            return false;
        }

        Function &get_method(name_t name) {
            for (auto &method : this->methods) {
                if (method.name == name) {
                    return method;
                }
            }

            throw MethodNotFound();
        }

        virtual name_t get_type_name() {
            return typeid(Type).name();
        }

        template <typename T>
        static bool is_same_type(Type *type) {
            return type->get_type_name() == typeid(T).name();
        }

        void add_method(const Function &method) {
            this->methods.push_back(method);
        }

        virtual void destroy(Object *object) = 0;
        virtual void free(Object *object) {}

        virtual ~Type() = default;
    };

    class Scope {
    public:
        std::unordered_map<name_t, scope_value_t> values;
        size_t refcounter = 1;

        void flop() {
            // does nothing
            // flop fo no hoe
        }

        scope_value_t xget_value(name_t name) {
            return this->values.at(name);
        }

        scope_value_t get_value(name_t name) {
            if (this->values.find(name) == this->values.end()) {
                throw NameError();
            }

            return this->xget_value(name);
        }

        Scope *create_scope(name_t linked_name);
        Object *create_object(name_t name, Object *object);

        void decref(name_t name);
        void xincref(name_t name);
        void destroy_scope();

        constexpr bool xdecref() {
            if (this->refcounter != 0) {
                this->refcounter--;
            }

            return this->refcounter == 0;
        }

        constexpr void xincref() {
            this->refcounter++;
        }

        bool is_value_exists(name_t name) {
            return this->values.find(name) != this->values.end();
        }
    };

    class Object {
    public:
        Type *type;
        Scope *scope;

        size_t refcounter = 1;

        explicit Object(Type *_type, Scope *_scope) : type(_type), scope(_scope) {

        }

        constexpr void xincref() {
            this->refcounter++;
        }

        constexpr bool xdecref() {
            if (this->refcounter != 0) {
                this->refcounter--;
            }

            return this->refcounter == 0;
        }
    };

    class TypeAllocator {
    public:
        std::unordered_map<name_t, Type *> types;

        template <typename T>
        T *allocate_or_get_type() {
            auto name = typeid(T).name();

            if (this->types.find(name) == this->types.end()) {
                return this->allocate_type<T>();
            }

            return this->types.at(name);
        }

        template <typename T>
        T *allocate_type() {
            auto type = new T;

            this->types[typeid(T).name()] = type;

            return type;
        }
    };
}


#endif //KANLANG_TYPES_HPP
