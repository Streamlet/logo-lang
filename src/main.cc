#include <stdio.h>
#include "frontend/parser/parser.h"

int main() {
  char buffer[100] = {};
  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    Parser p(buffer);
    if (p.parse()) {
      auto ast = p.result();
      print_result(ast.get());
      print_struct(ast.get());
      print_graphviz(ast.get());
    } else {
      printf("%s\n", p.error().c_str());
    }
  }
  return 0;
}
