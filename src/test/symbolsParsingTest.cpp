#include <gtest/gtest.h>
#include <iostream>

#include "addr2Symbol.cpp"

namespace addr2Symbol {
using std::cout, std::endl;
TEST(ArgsParserTest, TestMethodParsing) {
#ifdef __APPLE__
  auto function_name = "_function_example";
#elif defined(__linux__)
  auto function_name = "function_example";
#endif
  Addr2Symbol addr_2_symbol;
  auto var4_ptr = addr_2_symbol.getVariableAddress("var4");
  auto var_uninitialized_ptr = addr_2_symbol.getVariableAddress("var_uninitialized");
  auto function_address = addr_2_symbol.getFunctionAddress("function_example");
  auto inside_function_address = function_address;
  int (*function_example)() = (int (*)()) addr_2_symbol.getFunctionAddress("function_example");
  EXPECT_EQ(function_example(), -123456789);
  EXPECT_EQ(addr_2_symbol.getFunctionInfo(inside_function_address)->name, function_name); // corner case
  EXPECT_EQ(addr_2_symbol.getFunctionInfo(inside_function_address + 1)->name, function_name); // in between case
  EXPECT_EQ(*((int *) var4_ptr), 123456789);
  EXPECT_EQ(*((int *) var_uninitialized_ptr), -123456789);
}

TEST(ArgsParserTest, TestMethodParsingBelowCodeCase) {
  Addr2Symbol addr_2_symbol;
  auto func_name = addr_2_symbol.getFunctionInfo(0);
  EXPECT_EQ(func_name, nullptr);
}

TEST(ArgsParserTest, TestMethodParsingAboveCodeCase) {
  Addr2Symbol addr_2_symbol;
  auto func_name = addr_2_symbol.getFunctionInfo(9223372036854775807);
  EXPECT_EQ(func_name, nullptr);
}

TEST(ArgsParserTest, TestMethodParsingInBetweenLibrariesCase) {
  Addr2Symbol addr_2_symbol;
  auto func_addr = addr_2_symbol.getFunctionAddress("");
  EXPECT_GT(func_addr, 0);
  EXPECT_EQ(addr_2_symbol.getFunctionInfo(func_addr), nullptr);
}
}  // addr2Symbol
