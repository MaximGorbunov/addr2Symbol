#include "mockLibrary.h"

namespace addr2Symbol {

int var4 = 123456789;
static int static_int_variable = 1;
int MockLibrary::func1() {
  return ++var1 + 1;
}
int MockLibrary::func2() {
  return ++var2 + 2;
}
int MockLibrary::func3() {
  return ++var3 + 3;
}

int function_example() {
  return -123456789;
}
}  //addr2Symbol
