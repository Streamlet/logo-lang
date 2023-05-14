#include "visualize.h"
#include <cassert>

const char *COMPAROR_NAMES[] = {"=", "!=", "<", ">", "<=", ">="};
const char *UNARY_OPERATOR_NAMES[] = {"POS", "NEG"};
const char *BINARY_OPERATOR_NAMES[] = {"ADD", "SUB", "MUL", "DIV", "EXP", "MOD"};

std::size_t replace_all(std::string &inout, std::string_view what, std::string_view with) {
    std::size_t count{};
    for (std::string::size_type pos{}; inout.npos != (pos = inout.find(what.data(), pos, what.length()));
         pos += with.length(), ++count)
        inout.replace(pos, what.length(), with.data(), with.length());
    return count;
}

int print_graphviz_internal(int count, ASTNode *node) {
    int next = count + 1;
    switch (node->node_type) {
    case ASTNode::CODE: {
        printf("  n%d [ label=\"ROOT\" shape=circle ];\n", count);
        for (const auto &statement : ((ASTCode *)node)->statements->statements) {
            printf("  n%d->n%d;\n", count, next);
            next = print_graphviz_internal(next, statement.get());
        }
        break;
    }
    case ASTNode::CONDITION: {
        printf("  n%d [ label=\"OP: %s\" shape=circle ];\n", count, COMPAROR_NAMES[((ASTCondition *)node)->comparor]);
        printf("  n%d->n%d;\n", count, next);
        next = print_graphviz_internal(next, ((ASTCondition *)node)->left.get());
        printf("  n%d->n%d;\n", count, next);
        next = print_graphviz_internal(next, ((ASTCondition *)node)->right.get());
        break;
    }
    case ASTNode::GLOBAL_STATEMENT: {
        switch (((ASTGlobalStatement *)node)->global_statement_type) {
        case ASTGlobalStatement::PROCEDURE_DEFINE: {
            printf("  n%d [ label=\"DEF: %s\" shape=circle ];\n", count,
                   ((ASTProcedureDefine *)node)->procedure_name.c_str());
            printf("  n%d->n%d;\n", count, next);
            printf("  n%d [ label=\"PARAMS: %zu\" shape=circle ];\n", next,
                   ((ASTProcedureDefine *)node)->parameter_list->variable_list.size());
            int parameters = next;
            ++next;
            for (const auto &statement : ((ASTProcedureDefine *)node)->parameter_list->variable_list) {
                printf("  n%d->n%d;\n", parameters, next);
                next = print_graphviz_internal(next, statement.get());
            }
            printf("  n%d->n%d;\n", count, next);
            printf("  n%d [ label=\"STMTS: %zu\" shape=circle ];\n", next,
                   ((ASTProcedureDefine *)node)->statements->statements.size());
            int statements = next;
            ++next;
            for (const auto &statement : ((ASTProcedureDefine *)node)->statements->statements) {
                printf("  n%d->n%d;\n", statements, next);
                next = print_graphviz_internal(next, statement.get());
            }
            break;
        }
        case ASTGlobalStatement::STATEMENT: {
            switch (((ASTStatement *)node)->statement_type) {
            case ASTStatement::EXPPRESSION: {
                switch (((ASTExpression *)node)->expr_type) {
                case ASTExpression::NUMBER: {
                    printf("  n%d [ label=\"NUM: %lg\" shape=circle ];\n", count, ((ASTNumber *)node)->dval);
                    break;
                }
                case ASTExpression::STRING: {
                    std::string sval = ((ASTString *)node)->sval;
                    replace_all(sval, "\"", "\\\"");
                    printf("  n%d [ label=\"STR: %s\" shape=circle ];\n", count, sval.c_str());
                    break;
                }
                case ASTExpression::UNARY_OPERATOR: {
                    printf("  n%d [ label=\"%s)\" shape=circle ];\n", count,
                           UNARY_OPERATOR_NAMES[((ASTUnaryOperator *)node)->op]);
                    printf("  n%d->n%d\n", count, next);
                    next = print_graphviz_internal(next, ((ASTUnaryOperator *)node)->operand.get());
                    break;
                }
                case ASTExpression::BINARY_OPERATOR: {
                    printf("  n%d [ label=\"%s\" shape=circle ];\n", count,
                           BINARY_OPERATOR_NAMES[((ASTBinaryOperator *)node)->op]);
                    printf("  n%d->n%d;\n", count, next);
                    next = print_graphviz_internal(next, ((ASTBinaryOperator *)node)->left.get());
                    printf("  n%d->n%d;\n", count, next);
                    next = print_graphviz_internal(next, ((ASTBinaryOperator *)node)->right.get());
                    break;
                }
                case ASTExpression::PROCEDURE_CALL: {
                    printf("  n%d [ label=\"CALL: %s\" shape=circle ];\n", count,
                           ((ASTProcedureCall *)node)->procedure_name.c_str());
                    printf("  n%d->n%d;\n", count, next);
                    printf("  n%d [ label=\"ARGS: %zu\" shape=circle ];\n", next,
                           ((ASTProcedureCall *)node)->argument_list->expression_list.size());
                    int arguments = next;
                    ++next;
                    for (const auto &statement : ((ASTProcedureCall *)node)->argument_list->expression_list) {
                        printf("  n%d->n%d;\n", arguments, next);
                        next = print_graphviz_internal(next, statement.get());
                    }
                    break;
                }
                default: {
                    assert(false);
                    break;
                }
                }
                break;
            }
            case ASTStatement::IF_STATEMENT: {
                printf("  n%d [ label=\"IF\" shape=circle ];\n", count);
                printf("  n%d->n%d;\n", count, next);
                printf("  n%d [ label=\"COND\" shape=circle ];\n", next);
                int condition = next;
                ++next;
                next = print_graphviz_internal(condition, ((ASTIfStatement *)node)->condition.get());
                printf("  n%d->n%d;\n", count, next);
                printf("  n%d [ label=\"STMTS: %zu\" shape=circle ];\n", next,
                       ((ASTIfStatement *)node)->statements->statements.size());
                int statements = next;
                ++next;
                for (const auto &statement : ((ASTIfStatement *)node)->statements->statements) {
                    printf("  n%d->n%d;\n", statements, next);
                    next = print_graphviz_internal(next, statement.get());
                }
                break;
            }
            case ASTStatement::REPEAT_STATEMENT: {
                printf("  n%d [ label=\"LOOP\" shape=circle ];\n", count);
                printf("  n%d->n%d;\n", count, next);
                printf("  n%d [ label=\"COUNT\" shape=circle ];\n", next);
                int repeat_count = next;
                ++next;
                next = print_graphviz_internal(repeat_count, ((ASTRepeatStatement *)node)->repeat_count.get());
                printf("  n%d->n%d;\n", count, next);
                printf("  n%d [ label=\"STMTS: %zu\" shape=circle ];\n", next,
                       ((ASTRepeatStatement *)node)->statements->statements.size());
                int statements = next;
                ++next;
                for (const auto &statement : ((ASTRepeatStatement *)node)->statements->statements) {
                    printf("  n%d->n%d;\n", statements, next);
                    next = print_graphviz_internal(next, statement.get());
                }
                break;
            }
            default: {
                assert(false);
                break;
            }
            }
            break;
        }
        default: {
            assert(false);
            break;
        }
        }
        break;
    }
    case ASTNode::VARIABLE: {
        printf("  n%d [ label=\"VAR: %s\" shape=circle ];\n", count, ((ASTVariable *)node)->variable_name.c_str());
        break;
    }
    default: {
        assert(false);
        break;
    }
    }
    return next;
}

void print_graphviz(ASTNode *node) {
    printf("digraph {\n"
           "  rankdir=TD;\n");
    print_graphviz_internal(0, node);
    printf("}\n");
}
