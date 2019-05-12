/////
#ifndef PLANCK_FMTFWD_HPP
#define PLANCK_FMTFWD_HPP
#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <string_view>

#if defined(_MSC_VER)
// Define ssize_t inside of our namespace.
#if defined(_WIN64)
typedef __int64 __ssize_t;
#else
typedef long __ssize_t;
#endif
#endif

namespace base {
using ssize_t = __ssize_t;
namespace format_internal {
enum class ArgType {
  INTEGER, // short,int,
  UINTEGER,
  FLOAT,
  STRING,
  POINTER
};
struct FormatArg {
  FormatArg(char c) : at(ArgType::INTEGER) {
    integer.i = c; /// if caset to uint64_t
    integer.width = sizeof(char);
  }
  FormatArg(unsigned char c) : at(ArgType::UINTEGER) {
    integer.i = c;
    integer.width = sizeof(char);
  }
  FormatArg(signed short j) : at(ArgType::INTEGER) {
    integer.i = j;
    integer.width = sizeof(short);
  }
  FormatArg(unsigned short j) : at(ArgType::UINTEGER) {
    integer.i = j;
    integer.width = sizeof(short);
  }
  FormatArg(signed int j) : at(ArgType::INTEGER) {
    integer.i = j;
    integer.width = sizeof(int);
  }
  FormatArg(unsigned int j) : at(ArgType::UINTEGER) {
    integer.i = j;
    integer.width = sizeof(int);
  }
  FormatArg(signed long j) : at(ArgType::INTEGER) {
    integer.i = j;
    integer.width = sizeof(long);
  }
  FormatArg(unsigned long j) : at(ArgType::UINTEGER) {
    integer.i = j;
    integer.width = sizeof(long);
  }
  FormatArg(signed long long j) : at(ArgType::INTEGER) {
    integer.i = j;
    integer.width = sizeof(long long);
  }
  FormatArg(unsigned long long j) : at(ArgType::UINTEGER) {
    integer.i = j;
    integer.width = sizeof(long long);
  }
  FormatArg(float f) : at(ArgType::FLOAT) {
    floating.ld = f;
    floating.width = sizeof(float);
  }
  FormatArg(double d) : at(ArgType::FLOAT) {
    floating.ld = d;
    floating.width = (unsigned char)sizeof(double);
  }
  FormatArg(long double ld) : at(ArgType::FLOAT) {
    floating.ld = ld;
    floating.width = (unsigned char)sizeof(long double);
  }
  // A C-style text string. and string_view
  FormatArg(const wchar_t *str) : at(ArgType::STRING) {
    strings.data = (str == nullptr) ? L"(NULL)" : str;
    strings.len = (str == nullptr) ? sizeof("(NULL)") - 1 : wcslen(str);
  }
  template <typename Allocator>
  FormatArg( // NOLINT(runtime/explicit)
      const std::basic_string<wchar_t, std::char_traits<wchar_t>, Allocator>
          &str) {
    strings.data = str.data();
    strings.len = str.size();
  }
  FormatArg(std::wstring_view sv) : at(ArgType::STRING) {
    strings.data = sv.data();
    strings.len = sv.size();
  }

  // Any pointer value that can be cast to a "void*".
  template <class T> FormatArg(T *p) : ptr((void *)p), type(ArgType::POINTER) {}
  union {
    struct {
      int64_t i;
      unsigned char width;
    } integer;
    struct {
      long double ld;
      unsigned char width;
    } floating;
    struct {
      const wchar_t *data;
      size_t len;
    } strings;
    const void *ptr;
  };
  const ArgType at;
};
ssize_t StrFormatInternal(wchar_t *buf, size_t sz, std::wstring_view fmt,
                          const FormatArg *args, size_t max_args);
std::wstring StrFormatInternal(std::wstring_view fmt, const FormatArg *args,
                               size_t max_args);
} // namespace format_internal

template <typename... Args>
ssize_t StrFormat(wchar_t *buf, size_t N, const wchar_t *fmt, Args... args) {
  const internal::FormatArg arg_array[] = {args...};
  return internal::StrFormatInternal(buf, N, fmt, arg_array, sizeof...(args));
}

template <size_t N, typename... Args>
ssize_t StrFormat(wchar_t (&buf)[N], const wchar_t *fmt, Args... args) {
  // Use Arg() object to record type information and then copy arguments to an
  // array to make it easier to iterate over them.
  const internal::FormatArg arg_array[] = {args...};
  return internal::StrFormatInternal(buf, N, fmt, arg_array, sizeof...(args));
}

template <typename... Args>
std::wstring StrFormat(const wchar_t *fmt, Args... args) {
  const internal::FormatArg arg_array[] = {args...};
  return internal::StrFormatInternal(fmt, arg_array, sizeof...(args));
}

// Fast-path when we don't actually need to substitute any arguments.
ssize_t StrFormat(wchar_t *buf, size_t N, const wchar_t *fmt);
std::wstring StrFormat(const wchar_t *fmt);
template <size_t N>
inline ssize_t StrFormat(wchar_t (&buf)[N], const wchar_t *fmt) {
  return StrFormat(buf, N, fmt);
}

} // namespace base

#endif