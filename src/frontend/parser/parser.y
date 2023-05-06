%{
#include "frontend/parser/ast.h"
#include <stdio.h>
#include <string>
#include <memory>

extern int yylex();
int yyerror(const char* s);

std::unique_ptr<ASTNode> yy_result;
std::string yy_error;
%}

%code requires
{
struct ASTNode;
}

%union {
    double dval;
    struct {
        const char *str;
        int len;
    } sval;

    ASTNode *node;
}

%token ADD
%token SUB
%token MUL
%token DIV
%token EXP
%token MOD
%token LPAREN
%token RPAREN
%token COMMA
%token <dval> NUM
%token <sval> FUNC

%left ADD SUB
%left MUL DIV MOD
%right EXP

%type <node> expr term factor expee params

%%
line: expr { yy_result = std::unique_ptr<ASTNode>($1); }
    ;

expr: term { $$ = $1; }
    | ADD term { $$ = new ASTUnaryOperator(ASTUnaryOperator::POS, $2); }
    | SUB term { $$ = new ASTUnaryOperator(ASTUnaryOperator::NEG, $2); }
    | expr ADD term { $$ = new ASTBinaryOperator(ASTBinaryOperator::ADD, $1, $3); }
    | expr SUB term { $$ = new ASTBinaryOperator(ASTBinaryOperator::SUB, $1, $3); }
    ;

term: factor { $$ = $1; }
    | term MUL factor { $$ = new ASTBinaryOperator(ASTBinaryOperator::MUL, $1, $3); }
    | term DIV factor { $$ = new ASTBinaryOperator(ASTBinaryOperator::DIV, $1, $3); }
    | term MOD factor { $$ = new ASTBinaryOperator(ASTBinaryOperator::MOD, $1, $3); }
    ;

factor: expee { $$ = $1; }
      | expee EXP factor { $$ = new ASTBinaryOperator(ASTBinaryOperator::EXP, $1, $3); }
      ;

expee: NUM { $$ = new ASTNumber($1); }
     | LPAREN expr RPAREN  { $$ = $2; }
     | FUNC LPAREN params RPAREN { $$ = $3; auto err = ((ASTFunction *)$$)->assign_name($1.str, $1.len); if (!err.empty()) { yyerror(YY_(err.c_str())); YYERROR; } }
     ;

params: expr { $$ = new ASTFunction; ((ASTFunction *)$$)->add_argument($1); }
      | params COMMA expr { $$ = $1; ((ASTFunction *)$$)->add_argument($3); }
      ;

%%

extern void *yy_init_ctx(const char *yy_str);
extern void yy_free_ctx(void *yy_ctx);
extern const char *yy_get_current_text();

int yyerror(const char* s) {
    yy_error = s;
    yy_error += " current code: ";
    yy_error += yy_get_current_text();
    return 0;
}

std::unique_ptr<ASTNode> yy_get_result() {
    return std::move(yy_result);
}

const std::string &yy_get_error() {
    return yy_error;
}
