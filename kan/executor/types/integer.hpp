//
// Created by kvxmmu on 2/13/21.
//

#ifndef KANLANG_INTEGER_HPP
#define KANLANG_INTEGER_HPP

#include "types.hpp"

namespace Kan::Types {
    class IntegerObject : public Object {
    public:
        int value;

        explicit IntegerObject(int _value) : value(_value) {

        }
    };

    class IntegerType : public Type {
    public:
        static IntegerObject *add(const objects_pair &pair) {
            auto left = reinterpret_cast<IntegerObject *>(pair.first);
            auto right = reinterpret_cast<IntegerObject *>(pair.second);

            left->n_decref();
            right->n_decref();


        }

        void init_type() override {
            this->service_methods.emplace_back(IntegerType::add,
                                                "__add__");
        }

        name_t get_type_name() override {
            return "Integer";
        }

        size_t hash(void *ptr) override {
            return std::hash<int>()(*reinterpret_cast<int *>(ptr));
        }

        bool free(void *ptr) override {

        }
    };





}





#endif //KANLANG_INTEGER_HPP
