//
// Created by kvxmmu on 2/21/21.
//

#ifndef KANLANG_STDTYPES_HPP
#define KANLANG_STDTYPES_HPP

#include "../types.hpp"

namespace Kan::STD {
    using namespace Kan::Types;

    class Module : public Object {
    public:
        name_t name;

        explicit Module(name_t _name, Type *_type,
                        Scope *_scope) : name(_name), Object(_type, _scope) {
        }

        void add_object(name_t obj_name, Object *object) {
            this->scope->create_object(obj_name, object);
        }

        template <template<typename> typename Iterable>
        void add_objects(const Iterable<std::pair<name_t, Object *>> &iterable) {
            for (auto &pair : iterable) {
                this->scope->create_object(pair.first, pair.second);
            }
        }

        Object *xget_object(name_t obj_name) {
            return reinterpret_cast<Object *>(this->scope->xget_value(obj_name).ptr);
        }

        Object *get_object(name_t obj_name) {
            if (!this->scope->is_value_exists(obj_name)) {
                throw NameError();
            }

            return this->xget_object(obj_name);
        }

        void decref_objects() {
            for (auto &obj : this->scope->values) {
                this->scope->decref(obj.first);
            }
        }

    };

    class String : public Object {
    public:
        name_t string;

        String(Type *_type, Scope *_scope,
               const char *_string, size_t size) : Object(_type, _scope), string(_string, size) {

        }
    };

    class StringType : public Type {
    public:
        StringType() {
            this->add_method(Function(StringType::constructor,
                                      "__constructor__"));
            this->add_method(Function(StringType::print,
                                      "print"));
        }

        static Object *print(Object *self, const objects_t &args) {
            auto str = reinterpret_cast<String *>(self);

            std::cout << str->string << std::endl;

            return nullptr;
        }

        static Object *constructor(Object *self, const objects_t &args) {
            return args.at(0);
        }

        static String *constructor(StringType *_type, Scope *_scope,
                                   const char *source, size_t size) {
            auto str = new String(_type, _scope,
                                  source, size);

            return str;
        }

        void destroy(Object *object) override {

        }

        void free(Object *object) override {
            auto str = reinterpret_cast<String *>(object);

            str->string = "";
        }
    };

    class ModuleType : public Type {
    public:
        static Object *constructor(Object *self, const objects_t &args) {
            return nullptr;
        }

        ModuleType() {
            this->add_method(Function(ModuleType::getattribute,
                                      "__getattribute__"));
        }

        Module *create_module(name_t module_name) {
            auto module_scope = new Scope;
            auto module = new Module(module_name,
                                     this, module_scope);

            return module;
        }

        static Object *getattribute(Object *self, const objects_t &objects) {
            auto module = reinterpret_cast<Module *>(self);
            Object *object = nullptr;

            auto attribute = objects.at(0);

            if (!Type::is_same_type<StringType>(attribute->type)) {
                return nullptr;
            }

            auto string = reinterpret_cast<String *>(attribute);

            try {
                object = module->get_object(string->string);
            } catch (NameError &e) {
                return nullptr;
            }

            return object;
        }

        void destroy(Object *object) override {
            auto module = reinterpret_cast<Module *>(object);

            module->decref_objects();
        }
    };

    class FunctionObject : public Object {
    public:
        Function func;

        FunctionObject(Type *_type, Scope *_scope,
                       Function _func) : func(_func), Object(_type, _scope) {

        }
    };

    class FunctionType : public Type {
    public:
        static Object *call(Object *self, const objects_t &args) {
            auto func = reinterpret_cast<FunctionObject *>(self);

            return func->func.callback(self, args);
        } // proxy

        FunctionType() {
            this->add_method(Function(FunctionType::call,
                                      "__call__"));
        }

        FunctionObject *create_function(Scope *_scope, Function func) {
            return new FunctionObject(this, _scope, func);
        }

        void destroy(Object *object) override {

        }

        void free(Object *object) override {

        }
    };

}

#endif //KANLANG_TYPES_HPP
