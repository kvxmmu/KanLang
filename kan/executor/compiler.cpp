//
// Created by kvxmmu on 2/13/21.
//

#include "compiler.hpp"

// a(b+1, 7 - 3)
// a(b+1 7 - 3)

using vector_stream_t = Kan::Statements::CompileStream;
using operation_pair_t = Kan::Executor::operation_pair_t;
using priority_t = Kan::Executor::priority_t;

operation_pair_t Kan::Executor::get_operation(ast_iterator_t &it) {
    priority_t max_priority = 0;
    priority_t max_pos;
    ResponseType max_type = ERROR;

    size_t offset = 0;

    ast_objects_t::value_type first, second, third;

    Token firstk, secondk, thirdk;

    while (!it.is_done(offset)) {
        if (it.container.size() == 1) {
            return std::make_pair(DONE, offset);
        }

        first = it.peek(offset);

        if (is_operator(first)) {
            try {
                second = it.peek(offset+1u);
            } catch (std::runtime_error &e) {
                return std::make_pair(ERROR, 0u);
            }

            CHECK_AND_SET_PRIORITY(get_priority(first), offset, UNARY_OP)
            CHECK_AND_SET_PRIORITY(get_priority(second), offset, BRACKETS)

            offset++;

            continue;
        }

        try {
            second = it.peek(offset+1u);
        } catch (std::runtime_error &e) {
            break;
        }

        if (!is_operator(first) && second->is_tree(TreeType::BRACKETS)) {
            CHECK_AND_SET_PRIORITY(VHIGH_PRIORITY, offset, CALL)

            offset++;

            continue;
        }

        try {
            third = it.peek(offset+2u);
        } catch (std::runtime_error &e) {
            break;
        }

        if (third->is_tree()) {
            CHECK_AND_SET_PRIORITY(get_priority(third), offset+2u, BRACKETS)
        }

        if (first->is_tree()) {
            CHECK_AND_SET_PRIORITY(get_priority(first), offset, BRACKETS)
        }

        if (first->is_tree() && second->check_token_type(TokenTypes::DOT)) {
            CHECK_AND_SET_PRIORITY(VTOP_PRIORITY, offset, BRACKETS)
        }

        CHECK_AND_SET_PRIORITY(get_priority(second), offset, BINARY_OP)

        offset+=2;
    }

    return std::make_pair(max_type, max_pos);
}

bool Kan::Executor::is_operator(const ast_objects_t::value_type &object) {
    if (object->is_tree() || object->is_empty_token()) {
        return false;
    }

    switch (object->token.token_types) {
        case TokenTypes::MUL:
        case TokenTypes::DIV:
        case TokenTypes::PLUS:
        case TokenTypes::MINUS:
        case TokenTypes::POW:
        case TokenTypes::LESSER_OR_EQUAL:
        case TokenTypes::EQUALEQUAL:
        case TokenTypes::GREATER:
        case TokenTypes::LESSER:
            return true;

        default:
            return false;
    }
}

std::vector<Kan::ast_objects_t> Kan::Executor::split_by_token(const ast_objects_t &objects,
                                                              TokenTypes token_type) {
    std::vector<ast_objects_t> separated = {{}};

    for (auto &object : objects) {
        if (object->check_token_type(token_type)) {
            separated.emplace_back();
        } else {
            separated.back().push_back(object);
        }
    }

    if (separated.back().empty()) {
        separated.pop_back();
    }

    return separated;
}

void Kan::Executor::compile_brackets(Kan::Statements::CompileStream *stream,
        const ast_objects_t::value_type &obj, size_t pos,
        ast_objects_t &objects) {
    Kan::Executor::compile_expression(stream, obj->tree->objects);
    Kan::Executor::replace_object_with_empty_token(objects, pos, 1);
}

bool Kan::Executor::is_unary_operator(ast_objects_t::value_type &object) {
    return is_operator(object) && (object->token.token_types == TokenTypes::MINUS ||
                                   object->token.token_types == TokenTypes::PLUS);
}

uint32_t Kan::Executor::compile_expression(Kan::Statements::CompileStream *stream,
                                       ast_objects_t &objects, bool is_arguments) {
    if (is_arguments) {
        auto args = split_by_token(objects, TokenTypes::COMMA);

        for (auto &arg : args) {
            Kan::Executor::compile_expression(stream, arg,
                    false);
        }

        return args.size();
    }

    Kan::Executor::ast_iterator_t it(objects);

    while (!it.is_done(0)) {
        operation_pair_t operation = Kan::Executor::get_operation(it);

        auto pos = operation.second;
        auto response = operation.first;

        switch (response) {
            case BRACKETS: {
                auto obj = it.peek(pos);

                Kan::Executor::compile_brackets(stream, obj,
                        pos, objects);

                break;
            }

            case UNARY_OP:{
                auto operator_ = it.peek(pos);
                auto operand = it.peek(pos+1u);

                if (operand->is_tree()) {
                    Kan::Executor::compile_brackets(stream, operand,
                            pos+1u, objects);

                    break;
                } else if (!Kan::Executor::is_unary_operator(operator_)) {
                    throw std::runtime_error("ladno, operator is not an unary operator");
                }

                Kan::Executor::compile_literal(operand->token, stream);

                if (operator_->check_token_type(TokenTypes::PLUS)) {
                    stream->uplus();
                } else {
                    stream->uminus();
                }

                Kan::Executor::replace_object_with_empty_token(objects, pos, 2u);

                break;
            }

            case BINARY_OP: {
                auto first = it.peek(pos);
                auto second = it.peek(pos+1u);
                auto third = it.peek(pos+2u);

                if (first->is_tree()) {
                    Kan::Executor::compile_brackets(stream, first,
                                                    pos, objects);

                    break;
                }

                if (third->is_tree()) {
                    Kan::Executor::compile_brackets(stream, second,
                                                    pos+1u, objects);

                    break;
                }

                if (second->token.token_types != TokenTypes::DOT) {
                    Kan::Executor::compile_literal(third->token, stream);
                } else if (third->token.token_types != TokenTypes::ID) {
                    throw std::runtime_error("Syntax error 2u");
                }

                Kan::Executor::compile_literal(first->token, stream);

                switch (second->token.token_types) {
                    case TokenTypes::PLUS:
                        stream->plus();

                        break;

                    case TokenTypes::LESSER:
                        stream->lesser();

                        break;

                    case TokenTypes::GREATER:
                        stream->greater();

                        break;

                    case TokenTypes::EQUALEQUAL:
                        stream->equal();

                        break;

                    case TokenTypes::GREATER_OR_EQUAL:
                        stream->greater_or_equal();

                        break;

                    case TokenTypes::LESSER_OR_EQUAL:
                        stream->lesser_or_equal();

                        break;

                    case TokenTypes::MINUS:
                        stream->minus();

                        break;

                    case TokenTypes::DOT:
                        stream->get_attribute(third->token.token);

                        break;

                    case TokenTypes::MUL:
                        stream->mul();

                        break;

                    case TokenTypes::DIV:
                        stream->div();

                        break;

                    default:
                        break;
                }

                Kan::Executor::replace_object_with_empty_token(objects, pos, 3u);

                break;
            }

            case DONE: {
                auto obj = it.peek(pos);

                if (obj->is_tree()) {
                    Kan::Executor::compile_brackets(stream, obj,
                                                    pos+1u, objects);
                } else if (!obj->is_empty_token()) {
                    Kan::Executor::compile_literal(obj->token, stream);
                }

                it.next(it.container.size());

                break;
            }

            case CALL:{
                auto obj = it.peek(pos);
                auto args = it.peek(pos+1u);

                Kan::Executor::compile_literal(obj->token,
                        stream, false);
                auto args_count = Kan::Executor::compile_expression(stream, args->tree->objects,true);
                stream->call(args_count);

                Kan::Executor::replace_object_with_empty_token(objects, pos, 2u);

                break;
            }

            case ERROR:
                std::cout << "Abort is a sin" << std::endl;

                abort();

            default:
                break;
        }
    }

    return 0u;
}

void Kan::Executor::replace_object_with_empty_token(ast_objects_t &objects, size_t index,
                                                    size_t count) {
    objects.erase(objects.begin()+index, objects.begin()+index+count);

    objects.insert(objects.begin()+index, std::make_shared<AstObject>(nullptr));
}

priority_t Kan::Executor::get_operator_priority(const Token &token) {
    switch (token.token_types) {
        case TokenTypes::MINUS:
        case TokenTypes::PLUS:
            return MIDDLE_PRIORITY;

        case TokenTypes::GREATER_OR_EQUAL:
        case TokenTypes::LESSER_OR_EQUAL:
        case TokenTypes::GREATER:
        case TokenTypes::LESSER:
        case TokenTypes::EQUALEQUAL:
            return LOW_PRIORITY;

        case TokenTypes::MUL:
        case TokenTypes::DIV:
            return HIGH_PRIORITY;

        case TokenTypes::DOT:
            return TOP_PRIORITY;

        default:
            return LOW_PRIORITY;
    }
}

priority_t Kan::Executor::get_priority(const ast_objects_t::value_type &object) {
    if (object->is_tree(TreeType::BRACKETS)) {
        return VHIGH_PRIORITY;
    } else if (object->is_tree()) {
        throw Kan::Executor::SyntaxError();
    } else {
        return Kan::Executor::get_operator_priority(object->token);
    }
}

bool Kan::Executor::is_integral_type(const Token &token) {
    return token.token_types == TokenTypes::FLOAT
           || token.token_types == TokenTypes::INTEGER;
}

void Kan::Executor::compile_literal(const Token &literal, Kan::Statements::CompileStream *stream,
                                    bool is_attribute) {
    if (literal.token_types == TokenTypes::SKIP) {
        return;
    }

    if (Kan::Executor::is_integral_type(literal)) {
        if (literal.token_types == TokenTypes::FLOAT) {
            stream->push_integral(std::stof(literal.token));
        } else {
            stream->push_integral(std::stoi(literal.token));
        }
    } else if (literal.token_types == TokenTypes::STRING) {
        stream->push_string(literal.token);
    } else if (literal.token_types == TokenTypes::ID) {
        if (literal.token == "true") {
            stream->push_true();

            return;
        } else if (literal.token == "false") {
            stream->push_false();

            return;
        }

        if (is_attribute) {
            stream->get_attribute(literal.token);
        } else {
            stream->load_id(literal.token);
        }
    } else {
        throw std::runtime_error("slava fisting anal");
    }
}

size_t Kan::Executor::compile_function_signature(Kan::Statements::CompileStream *stream,
                                                 const ast_objects_t &objects, std::vector<std::string> &sig_go) {
    auto args = split_by_token(objects, TokenTypes::COMMA);
    size_t argcount = 0;

    for (auto &arg : args) {
        if (arg.empty()) {
            throw SyntaxError(1);
        }

        auto name = arg.at(0);

        if (name->is_tree() || !name->check_token_type(TokenTypes::ID)) {
            throw SyntaxError(2);
        }

        sig_go.insert(sig_go.begin(), name->token.token);
        stream->add_argument_signature(name->token.token);

        argcount++;
    }

    return argcount;
}

void Kan::Executor::compile(Kan::AstTree &tree, Kan::Statements::CompileStream *stream,
        bool create_scope, bool is_function, bool is_class) {
    ast_iterator_t it(tree.objects);

    if (create_scope) {
        stream->start_scope();
    }

    std::vector<stmt> statement_parsers = {
        stmt({
                 stmt_cond(TokenTypes::ID),
                 stmt_cond(TokenTypes::EQUAL),
                 stmt_cond(TokenTypes::SEMICOLON, true)
             }, StatementType::VARIABLE_ASSIGN),

        stmt({
                 stmt_cond(TokenTypes::ID, IF_NAME),
                 stmt_cond(TreeType::BRACKETS),
                 stmt_cond(TreeType::BRACES)
             }, StatementType::IF),

        stmt({
                 stmt_cond(TokenTypes::ID, ELSE_NAME),
                 stmt_cond(TokenTypes::ID, IF_NAME),
                 stmt_cond(TreeType::BRACES)
        }, StatementType::ELSEIF),

         stmt({
                stmt_cond(TokenTypes::ID, ELSE_NAME),
                stmt_cond(TreeType::BRACES)
             }, StatementType::ELSE),

        stmt({
                stmt_cond(TokenTypes::ID, FUNC_NAME),
                stmt_cond(TokenTypes::ID),
                stmt_cond(TreeType::BRACKETS),
                stmt_cond(TreeType::BRACES)
             }, StatementType::FUNCTION),
//
        stmt({
                 stmt_cond(TokenTypes::ID, CLASS_NAME),
                 stmt_cond(TokenTypes::ID),
                 stmt_cond(TreeType::BRACES)
             }, StatementType::CLASS),
//
//        stmt({
//                     stmt_cond(TokenTypes::ID, IMPORT_NAME),
//                     stmt_cond(TokenTypes::SEMICOLON, true)
//             }, StatementType::IMPORT),

        stmt({
            stmt_cond(TokenTypes::SEMICOLON, true)
        }, StatementType::EXPRESSION),
    };

    while (!it.is_done(0)) {
        for (auto &parser : statement_parsers) {
            auto condition = parser.check_condition(it);

            if (condition.success) {
                switch (parser.name) {
                    case StatementType::VARIABLE_ASSIGN: {
                        name_t name = condition.objects.at(0)->token.token;

                        condition.objects.erase(condition.objects.begin(),
                                                condition.objects.begin() + 2);

                        Kan::Executor::compile_expression(stream, condition.objects);
                        stream->set_name(name);

                        break;
                    }

                    case StatementType::IF: {
                        auto cond_check = condition.objects.at(1);
                        auto execute_block = condition.objects.at(2);

                        Kan::Executor::compile_expression(stream,
                                cond_check->tree->objects, false);

                        Kan::Statements::VectorCompileStream cstream;
                        Kan::Executor::compile(*execute_block->tree, &cstream);

                        stream->jmp_if_false(cstream.size());
                        cstream.seek(0);

                        stream->copy_stream_buffer(&cstream);
                        stream->pop_object(); // pop the boolean value

                        break;
                    }

                    case StatementType::FUNCTION:{
                        auto func_name = condition.objects.at(1)->token.token;
                        auto func_args = condition.objects.at(2)->tree->objects;
                        auto func_block = condition.objects.at(3)->tree;

                        Statements::VectorCompileStream cstream;
                        Statements::VectorCompileStream argstream;

                        std::vector<std::string> string_sig;
                        uint8_t argscount = Kan::Executor::compile_function_signature(&argstream, func_args, string_sig);



                        Kan::Executor::compile(*func_block, &cstream, true, true); // every function need his own scope!

                        cstream.seek(0);
                        argstream.seek(0);

                        if (!is_class) {
                            stream->define_function(cstream.size(),
                                                    func_name, argscount);
                        } else {
                            stream->add_class_method(func_name, cstream.size(), argscount);
                        }

                        stream->copy_stream_buffer(&argstream);
                        stream->copy_stream_buffer(&cstream);

                        break;
                    }

                    case ELSE: {
                        auto code_block = condition.objects.at(1);

                        Kan::Statements::VectorCompileStream cstream;
                        Kan::Executor::compile(*code_block->tree, &cstream,
                                false);
                        stream->else_block(cstream.size());

                        cstream.seek(0);
                        stream->copy_stream_buffer(&cstream);

                        stream->pop_object();

                        break;
                    }

                    case StatementType::EXPRESSION: {
                        Kan::Executor::compile_expression(stream,
                                condition.objects, false);

                        stream->pop_object();

                        break;
                    }

                    case StatementType::CLASS: {
                        auto class_name = condition.objects.at(1);
                        auto class_code = condition.objects.at(2);

                        Statements::VectorCompileStream cstream;
                        compile(*class_code->tree,
                                &cstream, false, false, true);

                        cstream.seek(0);

                        stream->create_class(class_name->token.token,
                                cstream.size());
                        stream->copy_stream_buffer(&cstream);

                        break;
                    }

                    default:


                        break;
                }

                break;
            }
        }
    }

    if (create_scope) {
        stream->end_scope();
    }

    if (is_function) {
        stream->ret_null();
    }
}

