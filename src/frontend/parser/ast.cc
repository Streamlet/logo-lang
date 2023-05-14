#include "ast.h"
#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <assert.h>
#include <math.h>
#include <sstream>

ASTNode::ASTNode(ASTNodeType node_type) : node_type(node_type) {
}

ASTNode::~ASTNode() {
}

ASTExpression::ASTExpression(ASTExpressionType expr_type)
    : ASTStatement(ASTStatement::EXPPRESSION), expr_type(expr_type) {
}

ASTExpressionList::ASTExpressionList() : ASTNode(EXPRESSION_LIST) {
}

ASTNumber::ASTNumber(double dval) : ASTExpression(NUMBER), dval(dval) {
}

ASTString::ASTString(const char *sval, int len) : ASTExpression(STRING), sval(sval, len) {
}

ASTUnaryOperator::ASTUnaryOperator(Operator op, ASTExpression *operand)
    : ASTExpression(UNARY_OPERATOR), op(op), operand(operand) {
}

ASTBinaryOperator::ASTBinaryOperator(Operator op, ASTExpression *left, ASTExpression *right)
    : ASTExpression(BINARY_OPERATOR), op(op), left(left), right(right) {
}

ASTVariable::ASTVariable(const char *variable_name, int variable_name_len)
    : ASTNode(VARIABLE), variable_name(variable_name, variable_name_len) {
}

ASTVariableList::ASTVariableList() : ASTNode(VARIABLE_LIST) {
}

ASTGlobalStatement::ASTGlobalStatement(GlobalStatementType global_statement_type)
    : ASTNode(GLOBAL_STATEMENT), global_statement_type(global_statement_type) {
}

ASTGlobalStatementList::ASTGlobalStatementList() : ASTNode(GLOBAL_STATEMENT_LIST) {
}

ASTStatement::ASTStatement(StatementType statement_type)
    : ASTGlobalStatement(STATEMENT), statement_type(statement_type) {
}

ASTStatementList::ASTStatementList() : ASTNode(STATEMENT_LIST) {
}

enum class PredifinedProcedureType {
    PRINT,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    ProcedureType_Count,
} procedure_type;

struct PredefinedProcedure {
    PredifinedProcedureType func;
    const char *name;
    int args;
};

PredefinedProcedure PREDEFINED_PROCEDURES[] = {
    {PredifinedProcedureType::PRINT, "print", 1},       {PredifinedProcedureType::FORWARD, "forward", 1},
    {PredifinedProcedureType::BACKWARD, "backward", 1}, {PredifinedProcedureType::LEFT, "left", 1},
    {PredifinedProcedureType::RIGHT, "right", 1},
};

ASTProcedureDefine::ASTProcedureDefine(const char *procedure_name, int procedure_name_len,
                                       ASTVariableList *parameter_list, ASTStatementList *statements)
    : ASTGlobalStatement(PROCEDURE_DEFINE), procedure_name(procedure_name, procedure_name_len),
      parameter_list(parameter_list), statements(statements) {
}

ASTProcedureCall::ASTProcedureCall(const char *procedure_name, int procedure_name_len, ASTExpressionList *argument_list)
    : ASTExpression(ASTExpression::PROCEDURE_CALL), procedure_name(procedure_name, procedure_name_len),
      argument_list(argument_list) {
}

ASTCondition::ASTCondition(Comparor comparor, ASTExpression *left, ASTExpression *right)
    : ASTNode(CONDITION), comparor(comparor), left(left), right(right) {
}

ASTIfStatement::ASTIfStatement(ASTCondition *condition, ASTStatementList *statements)
    : ASTStatement(IF_STATEMENT), condition(condition), statements(statements) {
}

ASTRepeatStatement::ASTRepeatStatement(ASTExpression *repeat_count, ASTStatementList *statements)
    : ASTStatement(REPEAT_STATEMENT), repeat_count(repeat_count), statements(statements) {
}

ASTCode::ASTCode(ASTGlobalStatementList *statements) : ASTNode(CODE), statements(statements) {
}
