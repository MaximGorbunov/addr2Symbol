#include "addr2Symbol.hpp"
#include "addr2Symbol_apple.inline.h"
#include "addr2Symbol_linux.inline.h"
#include <algorithm>

namespace addr2Symbol {

Addr2Symbol::Addr2Symbol() : functions(), variables() {
  load_symbols(this);
  std::sort(functions.begin(), functions.end());
}

intptr_t Addr2Symbol::getVariableAddress(const std::string &name) {
  try {
#ifdef __APPLE__
    return variables.at("_" + name);
#elif defined(__linux)
    return variables.at(name);
#endif
  } catch (const std::out_of_range &err) {
    return -1;
  }
}

intptr_t Addr2Symbol::getFunctionAddress(const std::string &name) {
#ifdef __APPLE__
  auto fixed_name = "_" + name;
  for (const auto &item : functions) {
    if (item.name == fixed_name) {
      return item.addr;
    }
  }
#elif defined(__linux)
  for (const auto &item : functions) {
    if (item.name == name) {
      return item.addr;
    }
  }
#endif
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

void Addr2Symbol::addFunction(const std::shared_ptr<std::string> &lib_name,
                              const std::string &function_name,
                              intptr_t address) {
  functions.push_back(function_info{lib_name, function_name, address});
}

void Addr2Symbol::addVariable(const std::string &function_name, intptr_t address) {
  variables.insert({function_name, address});
}
}  //addr2Symbol