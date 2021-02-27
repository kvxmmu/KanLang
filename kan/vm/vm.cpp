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

Object *Kan::VM::kanvm_execute(Kan::Statements::CompileStream *stream, vm_stdtypes_t *types,
                               bool inside_function) {
    std::stack<vm_stack_object_t> stack;
    std::deque<Kan::Memory::Scope *> scopes;

    auto &list_type = types->list_type;
    auto &int_type = types->int_type;
    auto &func_type = types->func_type;
    auto &string_type = types->string_type;
    auto &bool_type = types->bool_type;
    auto &user_func_type = types->user_func_type;

    vm_stack_object_t left{};
    vm_stack_object_t right{};

    bool running = true;

    while (stream->offset < stream->size() && running) {
        auto type = stream->read_integral<uint8_t>();

        switch (type) {
            case PLUSB:
            case MINUSB:
            case MULB:
            case DIVB:
            case UPLUS:
            case UMINUS:
            case EQUALB: {
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

                    case EQUALB:
                        object = new BoolObject(left_type->is_equals(left.object,
                                                right.object), &bool_type);

                        break;

                    default:

                        break;
                }

                if (left.is_literal) {
                    Object::safe_deallocate(left.object);
                }

                if (right.is_literal) {
                    Object::safe_deallocate(right.object);
                }

                if (left.is_literal || right.is_literal) {
                    stack.push({object, true, false});
                } else {
                    stack.push(object);
                }


                break;
            }

            case PUSH_TRUE:{
                stack.push(types->true_object);

                break;
            }

            case PUSH_FALSE: {
                stack.push(types->false_object);

                break;
            }

            case PUSH_INTEGRAL: {
                auto size = stream->read_integral<uint16_t>();
                auto integral = stream->read_integral<int>();
                auto int_object = new IntObject(&int_type, integral);

                stack.push({int_object, true, false});

                break;
            }

            case START_SCOPE: {
                auto scope = new Kan::Memory::Scope;

                if (types->global_scope == nullptr) {
                    types->global_scope = scope;
                    types->initializer(scope, types);
                }

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

                if (object.object != nullptr) {
                    object.object->xincref();
                }

                stack.pop();

                break;
            }

            case PUSH_STRING: {
                auto str = stream->get_string_with_size();
                auto string = new StringObject(str, &string_type);

                stack.push({string, true, str.empty()});

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

                getattr_callback_t getattr_cb;

                if (object_type->getattr == nullptr) {
                    getattr_cb = Kan::STD::getattr_default_callback;
                } else {
                    getattr_cb = object_type->getattr;
                }

                auto attr_object = getattr_cb(object.object, attr,
                        &func_type);

                stack.push(attr_object);

                if (object.is_literal) {
                    Object::safe_deallocate(object.object);
                }

                break;
            }

            case RET_NULL: {
                stack.push(nullptr);
                running = false;

                break;
            }

            case RET: {
                running = false;

                break;
            }

            case CALLB: {
                auto args_count = stream->read_integral<uint32_t>();

                std::vector<vm_stack_object_t> args;

                auto temp_list = new ListObject(&list_type);
                auto func_args = new ListObject(&list_type);

                for (size_t arg_num = 0; arg_num < args_count; arg_num++) {
                    auto argument = stack.top();

                    stack.pop();

                    args.push_back(argument);
                }

                auto object = stack.top();
                auto object_type = object.object->type;
                stack.pop();

                if (object.self != nullptr) {
                    func_args->objects.push_back(object.self);
                }

                for (auto &arg : args) {
                    func_args->objects.push_back(arg.object);
                }

                temp_list->objects.push_back(object.object);
                temp_list->objects.push_back(func_args);

                size_t before_state = stream->tell();

                auto response = object_type->call(temp_list, scopes.back(),
                        stream, types);

                stream->seek(before_state);

                temp_list->deallocate();
                func_args->deallocate();

                if (object.object->type->is_same_type(&func_type)) {
                    object.object->deallocate();
                }

                for (auto &arg : args) {
                    if (arg.is_literal) {
                        Object::safe_deallocate(arg.object);
                    }
                }

                args.clear();

                stack.push(response);

                break;
            }

            case POP_OBJECT: {
                auto obj = stack.top();

                if (obj.is_literal) {
                    Object::safe_deallocate(obj.object);
                } else if (obj.object != nullptr && obj.object->refs == 0) {
                    obj.object->deallocate();
                }

                stack.pop();

                break;
            }

            case JMP_BACK: {
                auto offset = stream->read_integral<uint32_t>();

                stream->seek(stream->tell()-offset);

                break;
            }

            case JMP_NEXT: {
                auto offset = stream->read_integral<uint32_t>();

                stream->seek(stream->tell()+offset);

                break;
            }

            case DEFINE_FUNCTION: {
                auto function_length = stream->read_integral<uint32_t>();
                auto name_size = stream->read_integral<uint8_t>();
                auto argscount = stream->read_integral<uint8_t>();
                auto function_name = stream->get_string(name_size);

                auto user_func_obj = new UserFunctionObject(0, function_length,
                                                            user_func_type, {});

                for (size_t argnum = 0; argnum < argscount; argnum++) {
                    auto sig = stream->read_argument_signature();

                    user_func_obj->signature.push_back(sig);
                }

                user_func_obj->address = stream->tell();
                scopes.back()->set_name(function_name, user_func_obj);

                stream->seek(stream->tell()+function_length);

                break;
            }

            case JMP_TO_ADDRESS: {
                auto address = stream->read_integral<uint64_t>();

                stream->seek(address);

                break;
            }

            case JMP_IF_FALSE: {
                auto offset = stream->read_integral<uint32_t>();
                auto top_obj = stack.top();

                if (top_obj.object == nullptr ||
                    !top_obj.object->type->get_bool(top_obj.object)) {
                    stream->seek(stream->tell()+offset);

                    break;
                }

                break;
            }

            case JMP_IF_TRUE: {
                auto offset = stream->read_integral<uint32_t>();
                auto top_obj = stack.top();

                if (top_obj.object != nullptr &&
                    top_obj.object->type->get_bool(top_obj.object)) {
                    stream->seek(stream->tell()+offset);
                }

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

    if (inside_function) {
        return stack.top().object;
    }

    while (!stack.empty()) {
        auto object = stack.top();

        stack.pop();

        Object::safe_deallocate(object.object);
    }

    return nullptr;
}