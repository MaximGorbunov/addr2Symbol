#include "addr2Symbol.hpp"
#include "addr2Symbol_apple.inline.h"
#include "addr2Symbol_linux.inline.h"
#include <algorithm>

namespace addr2Symbol {

Addr2Symbol::Addr2Symbol() : functions(), variables() {
  load_symbols(this);
  std::sort(functions.begin(), functions.end());
  for (size_t i = 1; i < functions.size() - 1; ++i) {
    if (functions[i].name.empty() && functions[i].addr == functions[i + 1].addr) {
    } else if (functions[i].name.empty() && functions[i].addr == functions[i - 1].addr) {
      functions.erase(functions.begin() + static_cast<long>(i));
    }
  }
}

intptr_t Addr2Symbol::getVariableAddress(const std::string &name) {
  try {
#ifdef __APPLE__
    return variables.at("_" + name);
#elif defined(__linux__)
    return variables.at(name);
#endif
  } catch (const std::out_of_range &err) {
    return 0;
  }
}

intptr_t Addr2Symbol::getFunctionAddress(const std::string &name) {
#ifdef __APPLE__
  auto fixed_name = "_" + name;
  if (name.empty()) {
    fixed_name = name;
  }
  for (const auto &item : functions) {
    if (item.name == fixed_name) {
      return item.addr;
    }
  }
#elif defined(__linux__)
  for (const auto &item : functions) {
    if (item.name == name) {
      return item.addr;
    }
  }
#endif
  return 0;
}

const function_info *Addr2Symbol::getFunctionInfo(intptr_t address) {
  auto bound = std::lower_bound(
      functions.begin(),
      functions.end(),
      function_info{std::make_shared<std::string>(""), "", address});
  if (bound == functions.end() || bound->name.empty() || (bound == functions.begin() && bound->addr > address)) { // end of lib code
    return nullptr;
  }
  if (bound->addr == address) { // corner case
    auto index = std::distance(functions.begin(), bound);
    return &functions[index];
  } else {
    auto index = std::distance(functions.begin(), bound);
    return &functions[index - 1];
  }
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