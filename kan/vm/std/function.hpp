//
// Created by kvxmmu on 2/26/21.
//

#ifndef KANLANG_FUNCTION_HPP
#define KANLANG_FUNCTION_HPP

#include "../type.hpp"
#include "list.hpp"

#include "function_obj.hpp"

namespace Kan::STD {
    using namespace Kan::VM;

    class FunctionType : public Type {
    public:
        static Object *_call(Object *args, void *, void *, void *) {
            auto arglist = reinterpret_cast<ListObject *>(args);
            auto self = reinterpret_cast<FunctionObject *>(arglist->at(0));
            auto func_args = arglist->at(1);

            return self->callback(func_args, nullptr, nullptr, 0);
        }

        FunctionType() {
            this->call = FunctionType::_call;
        }

        name_t get_type_name() override {
            return typeid(*this).name();
        }

        DEFAULT_FREE_FUNC(FunctionObject)
        DEFAULT_IS_SAME_FUNC()
    };

    static self_pair_t getattr_default_callback(Object *self, const name_t &attr, Type *function_type) {
        if (self->type->has_method(attr)) {
            return std::make_pair(self, new FunctionObject(self->type->methods.at(attr), self,
                    function_type));
        }

        return std::make_pair(self, nullptr);
    }
}

#endif //KANLANG_FUNCTION_HPP
