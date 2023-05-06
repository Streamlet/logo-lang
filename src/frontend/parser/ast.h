#pragma once

#include <memory>
#include <string>
#include <vector>

struct ASTNode {
  enum ASTNodeType {
    NUMBER = 0,
    UNARY_OPERATOR,
    BINARY_OPERATOR,
    FUNCTION,
  } type;
  ASTNode(ASTNodeType type);
  virtual ~ASTNode();
  virtual double value() = 0;
};

struct ASTNumber : public ASTNode {
  ASTNumber(double dval);
  double value() override;

  double dval;
};

struct ASTUnaryOperator : public ASTNode {
  enum Operator {
    POS = 0,
    NEG,
  };

  ASTUnaryOperator(Operator op, ASTNode* operand);
  double value() override;

  Operator op;
  std::unique_ptr<ASTNode> operand;
};

struct ASTBinaryOperator : public ASTNode {
  enum Operator {
    ADD = 0,
    SUB,
    MUL,
    DIV,
    EXP,
    MOD,
  };

  ASTBinaryOperator(Operator op, ASTNode* left, ASTNode* right);
  double value() override;

  Operator op;
  std::unique_ptr<ASTNode> left;
  std::unique_ptr<ASTNode> right;
};

struct ASTFunction : public ASTNode {
  enum Function {
    SQRT = 0,
    LOG,
    LN,
    LG,
    SIN,
    COS,
    TAN,
    COT,
  };

  ASTFunction();
  double value() override;
  void add_argument(ASTNode* argumemt);
  std::string assign_name(const char* name, int len);

  Function func;
  std::vector<std::unique_ptr<ASTNode>> arguments;
};

void print_result(ASTNode* node);
void print_struct(ASTNode* node);
void print_graphviz(ASTNode* node);
