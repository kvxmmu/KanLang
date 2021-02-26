//
// Created by kvxmmu on 2/25/21.
//

#ifndef KANLANG_TYPE_HPP
#define KANLANG_TYPE_HPP

#include <functional>
#include <string>
#include <memory>

#include <unordered_map>

#include "mem_allocator.hpp"

#define CLASSNAME typeid(*this).name()
#define DEFAULT_FREE_FUNC(ObjectType) void free(Object *object) override {\
    delete reinterpret_cast<ObjectType *>(object);\
}
#define DEFAULT_IS_SAME_FUNC() bool is_same_type(Type *_type) override {\
    return this == _type;\
}

#define DEFAULT_INIT() this->getattr = Kan::STD::getattr_default_callback;

namespace Kan::VM {
    class Object;
    class Type;

    typedef std::pair<Object *, Object *> self_pair_t;
    typedef std::string_view name_t;
    typedef Object *(*c_callback_t)(Object *);
    typedef self_pair_t(*getattr_callback_t)(Object *, name_t, Type *);
    typedef Object *(*getitem_t)(Object *);
    typedef Object *(*dir_t)(Object *);
    typedef Object *(*construct_t)(Object *);
    typedef void (*destruct_t)(Object *);
    typedef Object *(*add_t)(Object *, Object *);
    typedef Object *(*mul_t)(Object *, Object *);
    typedef Object *(*sub_t)(Object *, Object *);
    typedef Object *(*div_op_t)(Object *, Object *);

    class Type {
    public:
        std::unordered_map<name_t, c_callback_t> methods;

        getattr_callback_t getattr = nullptr;
        getitem_t getitem = nullptr;
        dir_t dir = nullptr;
        construct_t constructor = nullptr;
        destruct_t destructor = nullptr;
        add_t add = nullptr;
        mul_t mul = nullptr;
        sub_t sub = nullptr;
        div_op_t div = nullptr;
        c_callback_t call = nullptr;

        void add_method(name_t name, const c_callback_t &callback) {
            this->methods[name] = callback;
        }

        virtual name_t get_type_name() = 0;
        virtual void free(Object *object) = 0;

        virtual bool is_same_type(Type *_type) = 0;
        virtual bool has_method(name_t method) {
            return this->methods.find(method) != this->methods.end();
        }
    };

    class Object {
    public:
        Type *type;
        size_t refs = 1;

        Object(Type *_type) : type(_type) {}

        void deallocate() {
            this->type->free(this);
        }

        ~Object() {
            if (this->type->destructor != nullptr) {
                this->type->destructor(this);
            }
        }
    };

    using ObjectsAllocator = Memory::Allocator<Object>;

    static ObjectsAllocator allocator;
}

#endif //KANLANG_TYPE_HPP
