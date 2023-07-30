#ifdef __linux
#ifndef ADDR2SYMBOL_SRC_MAIN_ADDR2SYMBOL_APPLE_INLINE_H_
#define ADDR2SYMBOL_SRC_MAIN_ADDR2SYMBOL_APPLE_INLINE_H_

#include <memory>
#include <string>
#include <link.h>
#include <unordered_map>
#include <cstring>
#include <iostream>

#include "addr2Symbol.hpp"

namespace addr2Symbol {

static int libCallback(struct dl_phdr_info *info, size_t size, void *addr_2_symbol_ptr) {
  auto *addr_2_symbol = (Addr2Symbol *) addr_2_symbol_ptr;
  char *baseAddress = nullptr;
  for (int j = 0; j < info->dlpi_phnum; j++) {
    auto p_type = info->dlpi_phdr[j].p_type;
    if (p_type == PT_LOAD) {
      if (baseAddress == nullptr) {
        baseAddress = (char *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr);
        std::unique_ptr<char, decltype(free) *> shstrtab = std::unique_ptr<char, decltype(free) *>{
            nullptr,
            free
        };
        std::unique_ptr<char, decltype(free) *> strtab = std::unique_ptr<char, decltype(free) *>{
            nullptr,
            free
        };
        std::unique_ptr<Elf64_Sym, decltype(free) *> symtab = std::unique_ptr<Elf64_Sym, decltype(free) *>{
            nullptr,
            free
        };
        size_t symbolsCount = 0;
        auto *header = (Elf64_Ehdr *) baseAddress;
        auto headers = std::unique_ptr<Elf64_Shdr, decltype(free) *>{
            reinterpret_cast<Elf64_Shdr *>(malloc(header->e_shnum * sizeof(Elf64_Shdr))),
            free
        };
        auto fd = fopen(info->dlpi_name, "r");
        if (fd == nullptr) return 0;
        fseek(fd, (long) header->e_shoff, SEEK_SET);
        fread(headers.get(), sizeof(Elf64_Ehdr), header->e_shnum, fd);
        Elf64_Shdr &x = headers.get()[header->e_shstrndx];
        shstrtab.reset((char *) malloc(sizeof(char) * x.sh_size));
        fseek(fd, (long) x.sh_offset, SEEK_SET);
        fread(shstrtab.get(), 1, x.sh_size, fd);
        for (int index = 0; index < header->e_shnum; index++) {
          if (headers.get()[index].sh_type == SHT_STRTAB &&
              strcmp(".strtab", &shstrtab.get()[headers.get()[index].sh_name]) == 0) {
            strtab.reset((char *) malloc(sizeof(char) * headers.get()[index].sh_size));
            fseek(fd, (long) headers.get()[index].sh_offset, SEEK_SET);
            fread(strtab.get(), 1, headers.get()[index].sh_size, fd);
          }
          if (headers.get()[index].sh_type == SHT_SYMTAB) {
            symtab.reset((Elf64_Sym *) malloc(headers.get()[index].sh_size));
            symbolsCount = headers.get()[index].sh_size / headers.get()[index].sh_entsize;
            fseek(fd, (long) headers.get()[index].sh_offset, SEEK_SET);
            fread(symtab.get(), 1, headers.get()[index].sh_size, fd);
          }
        }
        for (size_t symIndex = 0; symIndex < symbolsCount; symIndex++) {
          char *symName = &strtab.get()[symtab.get()[symIndex].st_name];
          if (ELF64_ST_TYPE(symtab.get()[symIndex].st_info) == STT_FUNC) {
            addr_2_symbol->addFunction(std::make_shared<std::string>(info->dlpi_name),
                                       std::string(symName),
                                       (intptr_t) (symtab.get()[symIndex].st_value + baseAddress));
          } else if (ELF64_ST_TYPE(symtab.get()[symIndex].st_info) == STT_OBJECT) {
            addr_2_symbol->addVariable(std::string(symName),
                                       (intptr_t) (symtab.get()[symIndex].st_value + baseAddress));
          }
        }
        fclose(fd);
      }
    }
  }
  return 0;
}

inline static void load_symbols(addr2Symbol::Addr2Symbol *addr_2_symbol) {
  dl_iterate_phdr(libCallback, (void *) addr_2_symbol);
}
} // addr2Symbol
#endif //ADDR2SYMBOL_SRC_MAIN_ADDR2SYMBOL_APPLE_INLINE_H_
#endif //__linux
