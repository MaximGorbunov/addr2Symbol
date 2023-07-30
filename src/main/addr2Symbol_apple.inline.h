#ifndef ADDR2SYMBOL_SRC_MAIN_ADDR2SYMBOL_APPLE_INLINE_H_
#define ADDR2SYMBOL_SRC_MAIN_ADDR2SYMBOL_APPLE_INLINE_H_

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <cxxabi.h>
#include <memory>
#include <string>

#include "addr2Symbol.hpp"

namespace addr2Symbol {
inline static void load_symbols(std::vector<function_info> &functions,
                                std::unordered_map<std::string, intptr_t> &variables) {
  uint32_t images_count = _dyld_image_count();
  for (uint32_t image_index = 0; image_index < images_count; image_index++) {
    const auto image_name = std::make_shared<std::string>(_dyld_get_image_name(image_index));
    const mach_header_64 *header = (mach_header_64 *) _dyld_get_image_header(image_index);
    const ::intptr_t slide = _dyld_get_image_vmaddr_slide(image_index);
    uint32_t load_command_count = header->ncmds;
    auto *command_ptr = (load_command *) (header + 1);
    char *link_edit_segment;
    char *text_segment;
    char *data_segment;
    uint64_t sects_counter = 1;
    uint64_t text_segment_low_bound = 0;
    uint64_t text_segment_upper_bound = 0;
    uint64_t data_segment_low_bound = 0;
    uint64_t data_segment_upper_bound = 0;
    for (uint32_t j = 0; j < load_command_count; j++) {
      if (command_ptr->cmd == LC_SEGMENT_64) {
        auto command = (segment_command_64 *) command_ptr;
        if (strcmp(SEG_LINKEDIT, command->segname) == 0) {
          link_edit_segment = (char *) (command->vmaddr + slide - command->fileoff);
        }
        if (strcmp(SEG_TEXT, command->segname) == 0) {
          text_segment = (char *) (command->vmaddr + slide - command->fileoff);
          text_segment_low_bound = sects_counter;
          sects_counter += command->nsects;
          text_segment_upper_bound = sects_counter;
        }
        if (strcmp(SEG_DATA, command->segname) == 0) {
          data_segment = (char *) (command->vmaddr + slide - command->fileoff);
          data_segment_low_bound = sects_counter;
          sects_counter += command->nsects;
          data_segment_upper_bound = sects_counter;
        }
      }
      if (command_ptr->cmd == LC_SYMTAB) {
        auto *sym_tab_command = (symtab_command *) command_ptr;
        auto symbol_table_entry = (nlist_64 *) (link_edit_segment + sym_tab_command->symoff);
        auto string_table = (char *) link_edit_segment + sym_tab_command->stroff;
        for (uint32_t sym = 0; sym < sym_tab_command->nsyms; sym++) {
          nlist_64 &entry = symbol_table_entry[sym];
          if ((entry.n_type & (N_TYPE | N_STAB)) == N_SECT && entry.n_value != 0) { // defined non debug symbol
            auto mangled_name = std::string((char *) (string_table + entry.n_un.n_strx));
            if (entry.n_sect >= text_segment_low_bound && entry.n_sect < text_segment_upper_bound) {
              int status;
              auto demangled_name = abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status);
              auto address = reinterpret_cast<intptr_t>(text_segment + entry.n_value);
              if (status == 0) {
                functions.push_back(function_info{image_name, std::string(demangled_name), address});
              } else {
                functions.push_back(function_info{image_name, mangled_name, address});
              }
              free(demangled_name);
            } else if (entry.n_sect >= data_segment_low_bound && entry.n_sect < data_segment_upper_bound) {
              auto address = reinterpret_cast<intptr_t>(data_segment + entry.n_value);
              variables.insert({mangled_name, address});
            }
          }
        }
      }
      command_ptr = (load_command *) ((char *) command_ptr + command_ptr->cmdsize);
    }
  }
}
} // addr2Symbol
#endif
#endif //ADDR2SYMBOL_SRC_MAIN_ADDR2SYMBOL_APPLE_INLINE_H_
