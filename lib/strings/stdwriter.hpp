/////////
#ifndef PLANCK_STDWRITER_HPP
#define PLANCK_STDWRITER_HPP
#include <cstdio>
#include <string_view>
#include "fmt.hpp"

namespace base {

ssize_t StdWrite(FILE *out, std::wstring_view msg);
template <typename... Args>
ssize_t FPrintF(FILE *out, const wchar_t *fmt, Args... args) {
  const format_internal::FormatArg arg_array[] = {args...};
  auto str =
      format_internal::StrFormatInternal(fmt, arg_array, sizeof...(args));
  return 0;
}

} // namespace base

#endif