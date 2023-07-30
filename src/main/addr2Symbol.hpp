#ifndef ADDR2SYMBOL_SRC_MAIN_ADDR2LINPE_H_
#define ADDR2SYMBOL_SRC_MAIN_ADDR2LINPE_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace addr2Symbol {

struct function_info {
  std::shared_ptr<std::string> lib_name;
  std::string name;
  intptr_t addr;
  bool operator<(const function_info& other) const
  {
    return addr < other.addr ;
  }
};

class Addr2Symbol {
 public:
  Addr2Symbol();
  intptr_t getVariableAddress(const std::string &name);
  intptr_t getFunctionAddress(const std::string &name);
  std::string* getFunctionName(intptr_t address);
 private:
  std::vector<function_info> functions;
  std::unordered_map<std::string, intptr_t> variables;
};
}  //addr2Symbol
#endif //ADDR2SYMBOL_SRC_MAIN_ADDR2LINPE_H_
