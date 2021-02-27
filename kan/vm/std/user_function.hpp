//
// Created by kvxmmu on 2/27/21.
//

#ifndef KANLANG_USER_FUNCTION_HPP
#define KANLANG_USER_FUNCTION_HPP

#include <utility>

#include "../type.hpp"
#include "user_obj.hpp"

#include <sstream>

namespace Kan::STD {
    using namespace Kan::VM;

    class UserFunctionType : public Type {
    public:
        bool get_bool(Object *self) override {
            return true;
        }

        name_t get_type_name() override {
            return CLASSNAME;
        }

        static Object *_call(Object *args, void *_scope, void *_stream, void *vm_types) {
            auto scope = reinterpret_cast<Kan::Memory::Scope *>(_scope);
            auto arglist = reinterpret_cast<ListObject *>(args);
            auto self = reinterpret_cast<UserFunctionObject *>(arglist->at(0));

            auto stream = reinterpret_cast<Kan::Statements::CompileStream *>(_stream);
            stream->seek(self->address);

            auto ret_value = Kan::VM::kanvm_execute(stream, reinterpret_cast<vm_stdtypes_t *>(vm_types), true);

            return ret_value;
        }

        UserFunctionType() {
            this->call = UserFunctionType::_call;
        }

        std::string repr(Object *self) override {
            std::stringstream os;

            os << "<User function at 0x";
            os << std::hex << reinterpret_cast<UserFunctionObject *>(self)->address << ">";

            return os.str();
        }

        DEFAULT_FREE_FUNC(UserFunctionObject)
        DEFAULT_IS_SAME_FUNC()
    };
}

#endif //KANLANG_USER_FUNCTION_HPP
