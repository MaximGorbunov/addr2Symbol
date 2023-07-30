#include "addr2Symbol.hpp"
#include "addr2Symbol_apple.inline.h"

namespace addr2Symbol {

Addr2Symbol::Addr2Symbol() : functions(), variables() {
  load_symbols(functions, variables);
  std::sort(functions.begin(), functions.end());
}

intptr_t Addr2Symbol::getVariableAddress(const std::string &name) {
  try {
    return variables.at(name);
  } catch (const std::out_of_range &err) {
    return -1;
  }
}

intptr_t Addr2Symbol::getFunctionAddress(const std::string &name) {
  for (const auto &item : functions) {
    if (item.name == name) {
      return item.addr;
    }
  }
  return -1;
}

std::string *Addr2Symbol::getFunctionName(intptr_t address) {
  auto bound = std::lower_bound(
      functions.begin(),
      functions.end(),
      function_info{std::make_shared<std::string>(""), "", address});
  auto index = std::distance(functions.begin(), bound);
  return &functions[index - 1].name;
}
}  //addr2Symbol