#include "frontend/parser/parser.h"
#include "frontend/parser/visualize.h"
#include <stdio.h>

int main() {
    char buffer[100] = {};
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        Parser p(buffer);
        if (p.parse()) {
            auto ast = p.result();
            print_graphviz(ast.get());
        } else {
            printf("%s\n", p.error().c_str());
        }
    }
    return 0;
}
