#pragma once

#include <memory>
#include <string>
#include <vector>

struct ASTNode {
    enum ASTNodeType {
        CODE = 0,
        CONDITION,
        GLOBAL_STATEMENT,
        GLOBAL_STATEMENT_LIST,
        STATEMENT_LIST,
        VARIABLE,
        VARIABLE_LIST,
        EXPRESSION_LIST,

        ASTNodeType_Count,
    } node_type;
    explicit ASTNode(ASTNodeType node_type);
    virtual ~ASTNode();
};

struct ASTVariable : public ASTNode {
    explicit ASTVariable(const char *variable_name, int variable_name_len);

    std::string variable_name;
};

struct ASTVariableList : public ASTNode {
    explicit ASTVariableList();

    std::vector<std::unique_ptr<ASTVariable>> variable_list;
};

struct ASTGlobalStatement : public ASTNode {
    enum GlobalStatementType {
        PROCEDURE_DEFINE = 0,
        STATEMENT,
        GlobalStatementType_Count,
    } global_statement_type;
    explicit ASTGlobalStatement(GlobalStatementType global_statement_type);
};

struct ASTGlobalStatementList : public ASTNode {
    ASTGlobalStatementList();

    std::vector<std::unique_ptr<ASTGlobalStatement>> statements;
};

struct ASTStatement : public ASTGlobalStatement {
    enum StatementType {
        EXPPRESSION = 0,
        IF_STATEMENT,
        REPEAT_STATEMENT,
        StatementType_Count,
    } statement_type;
    explicit ASTStatement(StatementType statement_type);
};

struct ASTStatementList : public ASTNode {
    ASTStatementList();

    std::vector<std::unique_ptr<ASTStatement>> statements;
};

struct ASTProcedureDefine : public ASTGlobalStatement {
    ASTProcedureDefine(const char *procedure_name, int procedure_name_len, ASTVariableList *parameter_list,
                       ASTStatementList *statements);

    std::string procedure_name;
    std::unique_ptr<ASTVariableList> parameter_list;
    std::unique_ptr<ASTStatementList> statements;
};

struct ASTExpression : public ASTStatement {
    enum ASTExpressionType {
        NUMBER = 0,
        STRING,
        UNARY_OPERATOR,
        BINARY_OPERATOR,
        PROCEDURE_CALL,
        ASTExpressionType_Count,
    } expr_type;
    explicit ASTExpression(ASTExpressionType expr_type);
};

struct ASTExpressionList : public ASTNode {
    explicit ASTExpressionList();

    std::vector<std::unique_ptr<ASTExpression>> expression_list;
};

struct ASTNumber : public ASTExpression {
    explicit ASTNumber(double dval);

    double dval;
};

struct ASTString : public ASTExpression {
    explicit ASTString(const char *sval, int len);

    std::string sval;
};

struct ASTUnaryOperator : public ASTExpression {
    enum Operator {
        POS = 0,
        NEG,
        Operator_Count,
    };

    ASTUnaryOperator(Operator op, ASTExpression *operand);

    Operator op;
    std::unique_ptr<ASTExpression> operand;
};

struct ASTBinaryOperator : public ASTExpression {
    enum Operator {
        ADD = 0,
        SUB,
        MUL,
        DIV,
        EXP,
        MOD,
        Operator_Count,
    };

    ASTBinaryOperator(Operator op, ASTExpression *left, ASTExpression *right);

    Operator op;
    std::unique_ptr<ASTExpression> left;
    std::unique_ptr<ASTExpression> right;
};

struct ASTProcedureCall : public ASTExpression {
    explicit ASTProcedureCall(const char *procedure_name, int procedure_name_len, ASTExpressionList *argument_list);

    std::string procedure_name;
    std::unique_ptr<ASTExpressionList> argument_list;
};

struct ASTCondition : public ASTNode {
    enum Comparor {
        EQ = 0,
        NEQ,
        LT,
        GT,
        LTE,
        GTE,
        Comparor_Count,
    } comparor;

    ASTCondition(Comparor comparor, ASTExpression *left, ASTExpression *right);

    std::unique_ptr<ASTExpression> left;
    std::unique_ptr<ASTExpression> right;
};

struct ASTIfStatement : public ASTStatement {
    explicit ASTIfStatement(ASTCondition *condition, ASTStatementList *statements);

    std::unique_ptr<ASTCondition> condition;
    std::unique_ptr<ASTStatementList> statements;
};

struct ASTRepeatStatement : public ASTStatement {
    explicit ASTRepeatStatement(ASTExpression *repeat_count, ASTStatementList *statements);

    std::unique_ptr<ASTExpression> repeat_count;
    std::unique_ptr<ASTStatementList> statements;
};

struct ASTCode : public ASTNode {
    explicit ASTCode(ASTGlobalStatementList *statements);

    std::unique_ptr<ASTGlobalStatementList> statements;
};
