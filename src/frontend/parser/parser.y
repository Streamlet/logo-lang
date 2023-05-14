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

%token EOL

%token TO
%token END

%token IF
%token REPEAT

%token WORD

%token LBRACKET
%token RBRACKET
%token COLON

%token EQ
%token NEQ
%token LT
%token GT
%token LTE
%token GTE

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
%token <sval> ID
%token <sval> STRING

%left ADD SUB
%left MUL DIV MOD
%right EXP

%type <sval> procedure_name
%type <node> code global_statements global_statement procedure_define procedure_parameters procedure_parameter variable procedure_body statements statement procedure_call procedure_arguments procedure_argument if_statement if_condition if_body repeat_statement repeat_count repeat_body expr term factor exponent string

%%

code: global_statements { yy_result = std::unique_ptr<ASTCode>(new ASTCode((ASTGlobalStatementList *)$1)); }
    ;

global_statements: /* empty */ { $$ = new ASTGlobalStatementList; }
                 | global_statements global_statement { $$ = $1; ((ASTGlobalStatementList *)$$)->statements.push_back(std::unique_ptr<ASTGlobalStatement>((ASTGlobalStatement *)$2)); }
                 | global_statements EOL { $$ = $1; }
                 ;

global_statement: procedure_define { $$ = $1;}
                | statement { $$ = $1;}
                ;

procedure_define: TO procedure_name procedure_parameters EOL procedure_body END { $$ = new ASTProcedureDefine($2.str, $2.len, (ASTVariableList *)$3, (ASTStatementList *)$5);}
                ;

procedure_name: ID { $$ = $1; }
              ;

procedure_parameters: /* empty */ { $$ = new ASTVariableList; }
                    | procedure_parameters procedure_parameter { $$ = $1; ((ASTVariableList *)$$)->variable_list.push_back(std::unique_ptr<ASTVariable>((ASTVariable *)$2)); }
                    ;

procedure_parameter: variable { $$ = $1; }
                   ;

variable: COLON ID { $$ = new ASTVariable($2.str, $2.len); }
        ;

procedure_body: statements { $$ = $1; }
              ;

statements: /* empty */ { $$ = new ASTStatementList; }
          | statements statement { $$ = $1; ((ASTStatementList *)$$)->statements.push_back(std::unique_ptr<ASTStatement>((ASTStatement *)$2)); }
          | statements EOL { $$ = $1; }
          ;

statement: procedure_call EOL   { $$ = $1;}
         | if_statement          { $$ = $1;}
         | repeat_statement      { $$ = $1;}
         ;

if_statement: IF if_condition LBRACKET if_body RBRACKET { $$ = new ASTIfStatement((ASTCondition *)$2, (ASTStatementList *)$4); }
            ;

if_condition: expr EQ expr  { $$ = new ASTCondition(ASTCondition::EQ,  (ASTExpression *)$1, (ASTExpression *)$3); }
            | expr NEQ expr { $$ = new ASTCondition(ASTCondition::NEQ, (ASTExpression *)$1, (ASTExpression *)$3); }
            | expr LT expr  { $$ = new ASTCondition(ASTCondition::LT,  (ASTExpression *)$1, (ASTExpression *)$3); }
            | expr GT expr  { $$ = new ASTCondition(ASTCondition::GT,  (ASTExpression *)$1, (ASTExpression *)$3); }
            | expr LTE expr { $$ = new ASTCondition(ASTCondition::LTE, (ASTExpression *)$1, (ASTExpression *)$3); }
            | expr GTE expr { $$ = new ASTCondition(ASTCondition::GTE, (ASTExpression *)$1, (ASTExpression *)$3); }
            ;
                ;
if_body: statements { $$ = $1; }
       ;

repeat_statement: REPEAT repeat_count LBRACKET repeat_body RBRACKET { $$ = new ASTRepeatStatement((ASTExpression *)$2, (ASTStatementList *)$4); }
                ;

repeat_count: expr { $$ = $1; }
            ;

repeat_body: statements { $$ = $1; }
           ;

procedure_call: procedure_name procedure_arguments { $$ = new ASTProcedureCall($1.str, $1.len, (ASTExpressionList *)$2)}
              ;

procedure_arguments: /* empty */ { $$ = new ASTExpressionList; }
                   | procedure_arguments procedure_argument { $$ = $1; ((ASTExpressionList *)$$)->expression_list.push_back(std::unique_ptr<ASTExpression>((ASTExpression *)$2)); }
                   ;

procedure_argument: expr { $$ = $1; }
                  ;

expr: term { $$ = $1; }
    | ADD term { $$ = new ASTUnaryOperator(ASTUnaryOperator::POS, (ASTExpression *)$2); }
    | SUB term { $$ = new ASTUnaryOperator(ASTUnaryOperator::NEG, (ASTExpression *)$2); }
    | expr ADD term { $$ = new ASTBinaryOperator(ASTBinaryOperator::ADD, (ASTExpression *)$1, (ASTExpression *)$3); }
    | expr SUB term { $$ = new ASTBinaryOperator(ASTBinaryOperator::SUB, (ASTExpression *)$1, (ASTExpression *)$3); }
    ;

term: factor { $$ = $1; }
    | term MUL factor { $$ = new ASTBinaryOperator(ASTBinaryOperator::MUL, (ASTExpression *)$1, (ASTExpression *)$3); }
    | term DIV factor { $$ = new ASTBinaryOperator(ASTBinaryOperator::DIV, (ASTExpression *)$1, (ASTExpression *)$3); }
    | term MOD factor { $$ = new ASTBinaryOperator(ASTBinaryOperator::MOD, (ASTExpression *)$1, (ASTExpression *)$3); }
    ;

factor: exponent { $$ = $1; }
      | exponent EXP factor { $$ = new ASTBinaryOperator(ASTBinaryOperator::EXP, (ASTExpression *)$1, (ASTExpression *)$3); }
      ;

exponent: NUM { $$ = new ASTNumber($1); }
        | string { $$ = $1; }
        | variable { $$ = $1; }
        | LPAREN expr RPAREN  { $$ = $2; }
        | procedure_call { $$ = $1; }
        ;

string: WORD STRING { $$ = new ASTString($2.str, $2.len); }
      | WORD ID     { $$ = new ASTString($2.str, $2.len); }
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
