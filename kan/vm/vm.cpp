//
// Created by kvxmmu on 2/25/21.
//

#include "vm.hpp"

#include <iostream>

using namespace Kan::STD;

bool Kan::VM::is_empty_command(uint8_t command) {
    switch (command) {
        case PLUSB:
        case MINUSB:
        case POWB:
        case CALLB:
        case UMINUS:
        case UPLUS:
        case START_SCOPE:
        case END_SCOPE:
            return true;

        default:
            return false;
    }
}

void Kan::VM::kanvm_execute(Kan::Statements::CompileStream *stream) {
    std::stack<vm_stack_object_t> stack;
    std::deque<Kan::Memory::Scope *> scopes;

    ListType list_type;
    IntType int_type;
    FunctionType func_type;
    StringType string_type;


    vm_stack_object_t left{};
    vm_stack_object_t right{};

    while (stream->offset < stream->size()) {
        auto type = stream->read_integral<uint8_t>();

        switch (type) {
            case PLUSB:
            case MINUSB:
            case MULB:
            case DIVB:
            case UPLUS:
            case UMINUS: {
                left = stack.top();
                stack.pop();

                right = stack.top();
                stack.pop();

                auto left_type = left.object->type;
                Object *object = nullptr;

                switch (type) {
                    case PLUSB:
                        object = left_type->add(left.object,
                                right.object);

                        break;

                    case MINUSB:
                        object = left_type->sub(left.object,
                                right.object);

                        break;

                    case MULB:
                        object = left_type->mul(left.object,
                                right.object);

                        break;

                    case DIVB:
                        object = left_type->div(left.object,
                                right.object);

                        break;

                    default:

                        break;
                }

                if (left.is_literal) {
                    left.object->deallocate();
                }

                if (right.is_literal) {
                    right.object->deallocate();
                }

                if (left.is_literal || right.is_literal) {
                    stack.push({object, true});
                } else {
                    stack.push(object);
                }


                break;
            }

            case PUSH_INTEGRAL: {
                auto size = stream->read_integral<uint16_t>();
                auto integral = stream->read_integral<int>();
                auto int_object = new IntObject(&int_type, integral);

                stack.push({int_object, true});

                break;
            }

            case START_SCOPE: {
                auto scope = new Kan::Memory::Scope;

                if (!scopes.empty()) {
                    scope->parent_scope = scopes.back();
                }

                scopes.push_back(scope);

                break;
            }

            case LOAD_ID: {
                auto top_scope = scopes.back();
                auto name = stream->get_string_with_size();
                auto object = top_scope->get_name(name);

                stack.push(object);

                break;
            }

            case SET_NAME: {
                auto top_scope = scopes.back();
                auto name = stream->get_string_with_size();
                auto object = stack.top();

                top_scope->set_name(name, object.object);

                stack.pop();

                break;
            }

            case PUSH_STRING: {
                auto str = stream->get_string_with_size();
                auto string = new StringObject(str, &string_type);

                stack.push({string, true});

                break;
            }

            case END_SCOPE: {
                auto top_scope = scopes.back();
                scopes.pop_back();

                top_scope->decref_names();
                delete top_scope;

                break;
            }

            case GET_ATTRIBUTESB: {
                auto attr = stream->get_string_with_size();
                auto object = stack.top();
                auto object_type = object.object->type;
                stack.pop();

                getattr_callback_t getattr_cb = nullptr;

                if (object_type->getattr == nullptr) {
                    getattr_cb = Kan::STD::getattr_default_callback;
                } else {
                    getattr_cb = object_type->getattr;
                }

                auto attr_object = getattr_cb(object.object, attr,
                        &func_type);

                stack.push(attr_object);

                if (object.is_literal) {
                    object.object->deallocate();
                }

                break;
            }

            case CALLB: {
                auto object = stack.top();
                auto object_type = object.object->type;
                stack.pop();

                auto temp_list = new ListObject(&list_type);
                auto func_args = new ListObject(&list_type);

                if (object.self != nullptr) {
                    func_args->objects.push_back(object.self);
                }

                temp_list->objects.push_back(object.object);
                temp_list->objects.push_back(func_args);

                auto response = object_type->call(temp_list);

                temp_list->deallocate();
                func_args->deallocate();

                if (object.object->type->is_same_type(&func_type)) {
                    object.object->deallocate();
                }

                stack.push(response);

                break;
            }

            default: {
                std::cout << "Unknown command: " << static_cast<int>(type) << std::endl;

                if (!is_empty_command(type)) {
                    auto size = stream->read_integral<uint16_t>();

                    char *tempbuff = new char[size];

                    stream->read_from_stream(tempbuff, size);

                    delete[] tempbuff;
                }

                break;
            }
        }
    }

    while (!stack.empty()) {
        auto object = stack.top();

        stack.pop();

        object.object->deallocate();
    }
}