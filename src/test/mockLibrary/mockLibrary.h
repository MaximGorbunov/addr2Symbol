#ifndef ADDR2SYMBOL_SRC_TEST_MOCKLIBRARY_MOCKLIBRARY_H_
#define ADDR2SYMBOL_SRC_TEST_MOCKLIBRARY_MOCKLIBRARY_H_

namespace addr2Symbol {
extern "C" {
extern int var4;
extern int function_example();
}
class MockLibrary {
 private:
  int var1;
  int func1();
 protected:
  int var2;
  int func2();
 public:
  int var3;
  int func3();
};

}  //addr2Symbol
#endif //ADDR2SYMBOL_SRC_TEST_MOCKLIBRARY_MOCKLIBRARY_H_
