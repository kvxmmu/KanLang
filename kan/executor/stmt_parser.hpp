//
// Created by kvxmmu on 2/16/21.
//

#ifndef KANLANG_STMT_PARSER_HPP
#define KANLANG_STMT_PARSER_HPP

#include <string_view>
#include <vector>

#include "../parser.hpp"
#include "statements_class.hpp"

namespace Kan {
    namespace Executor {
        typedef decltype(AstTree::objects) ast_objects_type_t;
        typedef Iterator<ast_objects_type_t> ast_iterator_t;
    }

    class StatementCondition {
    public:
        union {
            TokenTypes token_type;
            TreeType tree_type;
        };

        AstType type;

        std::string_view string;

        bool until_statement = false;

        [[maybe_unused]] explicit StatementCondition(TokenTypes _token_type,
                std::string_view _string = "") : token_type(_token_type), type(AstType::TOKEN), string(_string) {

        }

        explicit StatementCondition(TokenTypes until_token,
                bool _until_statement) : until_statement(_until_statement), token_type(until_token) {

        }

        [[maybe_unused]] explicit StatementCondition(TreeType _tree_type) : tree_type(_tree_type), type(AstType::TREE) {

        }

        [[nodiscard]] constexpr bool is_token() const {
            return this->type == AstType::TOKEN;
        }
    };

    typedef std::vector<StatementCondition> conditions_t;
    typedef std::vector<std::shared_ptr<AstObject>> ast_objects_t;

    struct condition_resp_t {
        bool success;
        ast_objects_t objects;
    };

    template <typename StatementName>
    class StatementParser {
    public:
        conditions_t conditions;
        StatementName name;

        explicit StatementParser(const conditions_t &_conditions,
                const StatementName &_name) : name(_name) {
            this->insert_conditions(_conditions);
        }

        void insert_conditions(const conditions_t &conds) {
            for (auto &condition : conds) {
                this->conditions.push_back(condition);
            }
        }

        condition_resp_t check_condition(Executor::ast_iterator_t &it) const {
            size_t offset = 0;
            bool until_condition = false;

            TokenTypes until_token;

            ast_objects_t objects;

            for (; offset < this->conditions.size(); offset++) {
                auto object = it.peek(offset);
                auto condition = this->conditions.at(offset);

                if (condition.until_statement) {
                    until_condition = true;
                    until_token = condition.token_type;

                    break;
                }

                if (condition.is_token() && object->is_tree()
                    || (!condition.is_token() && !object->is_tree())) {
                    return {false, {}};
                }

                if ((condition.is_token() && object->check_token_type(condition.token_type))
                    || !condition.is_token() && object->tree->type == condition.tree_type) {
                    if (!condition.string.empty() && condition.is_token() && !object->is_tree()) {
                        auto &token = object->token;

                        if (token.token != condition.string) {
                            break;
                        }
                    }

                    objects.push_back(object);
                } else {
                    break;
                }
            }

            while (until_condition) {
                std::shared_ptr<AstObject> object;

                try {
                    object = it.peek(offset++);
                } catch (std::runtime_error &e) {
                    return {false, {}};
                }

                if (object->check_token_type(until_token)) {
                    break;
                }

                objects.push_back(object);
            }

            it.next(offset);

            return {true, objects};
        }
    };
}

#endif //KANLANG_STMT_PARSER_HPP
