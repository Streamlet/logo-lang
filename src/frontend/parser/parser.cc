#include "parser.h"

void *yy_init_ctx(const char *yy_str);
void yy_free_ctx(void *yy_ctx);
int yyparse();
std::unique_ptr<ASTNode> yy_get_result();
const std::string &yy_get_error();

Parser::Parser(const char *yy_str) {
    yy_ctx = yy_init_ctx(yy_str);
}

Parser::~Parser() {
    yy_free_ctx(yy_ctx);
}

bool Parser::parse() {
    return yyparse() == 0;
}

std::unique_ptr<ASTNode> Parser::result() {
    return std::move(yy_get_result());
}

const std::string &Parser::error() {
    return yy_get_error();
}
