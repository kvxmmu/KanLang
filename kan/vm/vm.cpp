//
// Created by kvxmmu on 3/7/21.
//

#include "vm.hpp"

Object *Kan::VM::runcode(Kan::Statements::CompileStream *stream, vm_meta_t *meta,
                         bool is_function, const std::vector<std::pair<std::string, Object *>> *argnames) {
    std::deque<Kan::Memory::Scope *> scopes;
    std::stack<Object *> stack;

    size_t length = stream->size();

    if (meta->global_scope != nullptr) {
        scopes.push_back(meta->global_scope);
    }

    while (length > stream->tell()) {
        auto type = stream->read_integral<uint8_t>();

        switch (type) {
            case PUSH_INTEGRAL:{
                auto integral_type = stream->read_integral<uint8_t>();
                auto integral = stream->read_integral<uint32_t>();
                Object *object = nullptr;

                if (integral_type) {
                    float rfloat = 0.f;

                    memcpy(&rfloat, &integral, sizeof(uint32_t));

                    object = KanInt_FromFloat(rfloat);
                } else {
                    object = KanInt_FromInt(integral);
                }

                KanObject_Incref(object);

                stack.push(object);

                break;
            }

            case PUSH_STRING: {
                auto str = stream->get_string_with_size();
                auto str_obj = KanString_FromString(str);

                KanObject_Incref(str_obj);

                stack.push(str_obj);

                break;
            }

            case START_SCOPE: {
                auto scope = new Kan::Memory::Scope;

                if (meta->global_scope == nullptr) {
                    meta->global_scope = scope;

                    meta->global_scope_callback(scope);
                }

                if (!scopes.empty()) {
                    scope->parent_scope = scopes.back();
                }

                if (argnames != nullptr) {
                    auto &args = *argnames;

                    for (auto &arg : args) {
                        scope->set_name(arg.first, arg.second);
                    }
                }

                scopes.push_back(scope);

                break;
            }

            case RET: {
                auto top = stack.top();

                break;
            }

            case PLUSB:
            case MINUSB:
            case MULB:
            case DIVB:
            case LEFT_SHIFTB:
            case RIGHT_SHIFTB:
            case BIN_ORB:
            case BIN_XORB:
            case BIN_ANDB: {
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

                    case BIN_ORB: {
                        result = lt->bin_or(left, right);

                        break;
                    }

                    case LEFT_SHIFTB: {
                        result = lt->left_shift(left, right);

                        break;
                    }

                    case BIN_ANDB: {
                        result = lt->bin_and(left, right);

                        break;
                    }

                    case BIN_XORB: {
                        result = lt->bin_xor(left, right);

                        break;
                    }

                    case RIGHT_SHIFTB: {
                        result = lt->right_shift(left, right);

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

            case UPLUS:
            case UMINUS:
            case BIN_NOTB: {
                auto top = stack.top();
                stack.pop();

                Object *result = nullptr;

                switch (type) {
                    case UPLUS: {
                        result = top->type->pos(top);

                        break;
                    }

                    case UMINUS: {
                        result = top->type->neg(top);

                        break;
                    }

                    case BIN_NOTB: {
                        result = top->type->bin_not(top);

                        break;
                    }

                    default:
                        break;
                }

                KanObject_Decref(top);
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
                auto top = stack.top();

                KanObject_XDecref(top);

                scopes.back()->set_name(name, top);
                stack.pop();

                break;
            }

            case END_SCOPE: {
                scopes.back()->decref_names();
                delete scopes.back();

                scopes.pop_back();

                break;
            }

            case RET_NULL: {
                stack.push(meta->none_obj);
                KanObject_Incref(meta->none_obj);

                break;
            }

            case DEFINE_FUNCTION: {
                auto function_length = stream->read_integral<uint32_t>();
                auto name_size = stream->read_integral<uint8_t>();
                auto argscount = stream->read_integral<uint8_t>();
                auto name = stream->get_string(name_size);

                std::vector<std::string> signature;

                for (uint8_t pos = 0; pos < argscount; pos++) {
                    signature.push_back(stream->read_argument_signature());
                }

                auto object = KanFunction_FromAddr(stream->tell(), function_length, signature);
                scopes.back()->set_name(name, object);

                stream->seek(stream->tell()+function_length);

                break;
            }

            case LOAD_ID: {
                auto id = stream->get_string_with_size();
                auto object = scopes.back()->lookup_name(id);

                stack.push(object);
                KanObject_Incref(object);

                break;
            }

            case CALLB: {
                auto argscount = stream->read_integral<uint8_t>();
                Object *arglist = KanList_Create();

                for (uint8_t pos = 0; pos < argscount; pos++) {
                    KanList_Insert(arglist, stack.top(), 0);
                    stack.pop();
                }

                auto callable = stack.top();
                stack.pop();

                auto response = callable->type->call(callable, arglist, nullptr, meta, stream->get_buffer());

                if (response == meta->none_obj) {
                    KanObject_Incref(response);
                }

                stack.push(response);
                KanObject_Decref(arglist);
                KanObject_Decref(callable);

                break;
            }

            case PUSH_TRUE: {
                stack.push(meta->true_obj);
                KanObject_Incref(meta->true_obj);

                break;
            }

            case PUSH_FALSE: {
                stack.push(meta->false_obj);
                KanObject_Incref(meta->false_obj);

                break;
            }

            case PUSH_NONE: {
                stack.push(meta->none_obj);
                KanObject_Incref(meta->none_obj);

                break;
            }

            default:{

                break;
            }
        }
    }

    Object *ret = nullptr;

    if (is_function) {
        ret = stack.top();
        stack.pop();
    }

    for (auto cscope : scopes) {
        if (is_function && cscope == meta->global_scope) {
            continue;
        }

        cscope->decref_names();

        delete cscope;
    }

    if (ret != nullptr) {
    }

    if (!is_function) {
        type_allocator.deallocate_all();
    }

    return ret;
}

extern Object *execfunc(const char *buffer, size_t buffer_length, void *meta,
        const std::vector<std::pair<std::string, Object *>> &args) {
    Kan::Statements::VectorCompileStream stream;
    Object *ret = nullptr;

    stream.write_to_stream(buffer, buffer_length);
    stream.seek(0);

    ret = runcode(&stream, reinterpret_cast<Kan::VM::vm_meta_t *>(meta), true, &args);

    return ret;
}

extern Object *get_true(void *ptr) {
    return reinterpret_cast<Kan::VM::vm_meta_t *>(ptr)->true_obj;
}

extern Object *get_false(void *ptr) {
    return reinterpret_cast<Kan::VM::vm_meta_t *>(ptr)->false_obj;
}

extern Object *get_none(void *ptr) {
    return reinterpret_cast<Kan::VM::vm_meta_t *>(ptr)->none_obj;
}
