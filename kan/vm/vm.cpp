//
// Created by kvxmmu on 3/7/21.
//

#include "vm.hpp"

void Kan::VM::runcode(Kan::Statements::CompileStream *stream, Kan::Memory::Scope *scope) {
    std::deque<Kan::Memory::Scope *> scopes;
    std::stack<Object *> stack;

    size_t length = stream->size();

    while (length > stream->tell()) {
        auto type = stream->read_integral<uint8_t>();

        switch (type) {
            case PUSH_INTEGRAL:{
                auto integral_type = stream->read_integral<uint8_t>();
                auto integral = stream->read_integral<uint32_t>();

                if (integral_type) {
                    stack.push(KanInt_FromFloat(integral));
                } else {
                    stack.push(KanInt_FromInt(integral));
                }

                break;
            }

            case START_SCOPE: {
                scopes.push_back(new Kan::Memory::Scope);

                break;
            }

            case PLUSB:
            case MINUSB:
            case MULB:
            case DIVB:{
                auto right = stack.top();
                stack.pop();
                auto left = stack.top();
                stack.pop();

                auto lt = left->type;

                Object *result = nullptr;

                switch (type) {
                    case PLUSB: {
                        result = lt->add(left, right);

                        break;
                    }

                    case MINUSB: {
                        result = lt->sub(left, right);

                        break;
                    }

                    case MULB: {
                        result = lt->mul(left, right);

                        break;
                    }

                    case DIVB: {
                        result = lt->div(left, right);

                        break;
                    }

                    default:
                        break;
                }

                KanObject_Decref(right);
                KanObject_Decref(left);
                KanObject_Incref(result);

                stack.push(result);

                break;
            }

            case POP_OBJECT: {
                auto top = stack.top();
                stack.pop();

                KanObject_Decref(top);

                break;
            }

            case SET_NAME:{
                auto name = stream->get_string_with_size();

                scopes.back()->set_name(name, stack.top());
                stack.pop();

                break;
            }

            case END_SCOPE: {
                scopes.back()->decref_names();
                delete scopes.back();

                scopes.pop_back();

                break;
            }

            default:{


                break;
            }
        }
    }

    delete scope;

    for (auto cscope : scopes) {
        cscope->decref_names();

        delete cscope;
    }

    type_allocator.deallocate_all();
}
