////////////////
#ifndef PROBE_FWD_HPP
#define PROBE_FWD_HPP
#include <string>
#include <map>
#include <string_view>

namespace probe {
constexpr const int einident = 16;
struct elf_minutiae_t {
  uint8_t ident[einident];
};
std::wstring fromutf8(std::string_view text);
} // namespace probe

#endif