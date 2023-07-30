#include <gtest/gtest.h>
#include <iostream>

#include "addr2Symbol.cpp"

namespace addr2Symbol {
using std::cout, std::endl;
TEST(ArgsParserTest, TestMethodParsing) {
  Addr2Symbol addr_2_symbol;
  auto var4_ptr = addr_2_symbol.getVariableAddress("var4");
  auto function_address = addr_2_symbol.getFunctionAddress("function_example");
  auto inside_function_address =function_address + 1;
  int (*function_example)() = (int (*)()) addr_2_symbol.getFunctionAddress("function_example");
  EXPECT_EQ(-123456789, function_example());
#ifdef __APPLE__
  EXPECT_EQ("_function_example", *addr_2_symbol.getFunctionName(inside_function_address));
#elif defined(__linux__)
  EXPECT_EQ("function_example", *addr_2_symbol.getFunctionName(inside_function_address));
#endif
  EXPECT_EQ(123456789, *((int *) var4_ptr));
}
}  // addr2Symbol
