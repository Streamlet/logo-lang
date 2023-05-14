#pragma once

#include "ast.h"
#include <memory>

class Parser {
public:
    Parser(const char *yy_str);
    ~Parser();

    bool parse();
    std::unique_ptr<ASTNode> result();
    const std::string &error();

private:
    void *yy_ctx;
};
