////////////
#ifndef PLANCK_MAPVIEW_HPP
#define PLANCK_MAPVIEW_HPP
#pragma once
#ifndef _WINDOWS_
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN //
#endif
#include <windows.h>
#endif
#include <algorithm>
#include <string>
#include <string_view>

#ifndef _M_X64
class FsDisableRedirection {
public:
  typedef BOOL WINAPI fntype_Wow64DisableWow64FsRedirection(PVOID *OldValue);
  typedef BOOL WINAPI fntype_Wow64RevertWow64FsRedirection(PVOID *OldValue);
  FsRedirection() {
    auto pfnWow64DisableWow64FsRedirection =
        (fntype_Wow64DisableWow64FsRedirection *)GetProcAddress(
            GetModuleHandleW(L"kernel32.dll"),
            "Wow64DisableWow64FsRedirection");
    if (pfnWow64DisableWow64FsRedirection) {
      pfnWow64DisableWow64FsRedirection(&OldValue);
    }
  }
  ~FsRedirection() {
    auto pfnWow64RevertWow64FsRedirection =
        (fntype_Wow64RevertWow64FsRedirection *)GetProcAddress(
            GetModuleHandleW(L"kernel32.dll"), "Wow64RevertWow64FsRedirection");
    if (pfnWow64RevertWow64FsRedirection) {
      pfnWow64RevertWow64FsRedirection(&OldValue);
    }
  }

private:
  PVOID OldValue = nullptr;
};
#endif

namespace planck {
constexpr auto nullfile_t = INVALID_HANDLE_VALUE;
constexpr auto sizemax_t = (std::numeric_limits<std::size_t>::max)();

class memview {
public:
  static constexpr std::size_t npos = sizemax_t;
  memview() = default;
  memview(const char *d, std::size_t l) : data_(d), size_(l) {}
  memview(const memview &other) {
    data_ = other.data_;
    size_ = other.size_;
  }
  bool startswith(memview mv) {
    if (mv.size_ > size_) {
      return false;
    }
    return (memcmp(data_, mv.data_, mv.size_) == 0);
  }
  bool indexswith(std::size_t offset, std::string_view sv) const {
    if (offset > size_) {
      return false;
    }
    return memcmp(data_ + offset, sv.data(), sv.size()) == 0;
  }
  memview submv(std::size_t pos, std::size_t n = npos) {
    return memview(data_ + pos, (std::min)(n, size_ - pos));
  }
  std::size_t size() const { return size_; }
  const char *data() const { return data_; }
  unsigned char operator[](const std::size_t off) const {
    if (off >= size_) {
      return UCHAR_MAX;
    }
    return (unsigned char)data_[off];
  }
  template <typename T> const T *cast(size_t off) {
    if (off + sizeof(T) >= size_) {
      return nullptr;
    }
    return reinterpret_cast<const T *>(data_ + off);
  }

private:
  const char *data_{nullptr};
  std::size_t size_{0};
};

class mapview {
public:
  static void Close(HANDLE handle) {
    if (handle != nullfile_t) {
      CloseHandle(handle);
    }
  }

  mapview() = default;
  mapview(const mapview &) = delete;
  mapview &operator=(const mapview &) = delete;
  ~mapview() {
    if (data_ != nullptr) {
      ::UnmapViewOfFile(data_);
    }
    Close(FileMap);
    Close(FileHandle);
  }
  bool mapfile(std::wstring_view file, std::size_t minsize = 1,
               std::size_t maxsize = sizemax_t) {
#ifndef _M_X64
    FsDisableRedirection fdr;
#endif
    if ((FileHandle = CreateFileW(file.data(), GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                  nullptr)) == nullfile_t) {
      return false;
    }
    LARGE_INTEGER li;
    if (GetFileSizeEx(FileHandle, &li) != TRUE || li.QuadPart < minsize) {
      return false;
    }
    if ((FileMap = CreateFileMappingW(FileHandle, nullptr, PAGE_READONLY, 0, 0,
                                      nullptr)) == nullfile_t) {
      return false;
    }
    size_ = (size_t)li.QuadPart > maxsize ? maxsize : (size_t)li.QuadPart;
    auto baseAddr = MapViewOfFile(FileMap, FILE_MAP_READ, 0, 0, size_);
    if (baseAddr == nullptr) {
      return false;
    }
    data_ = reinterpret_cast<char *>(baseAddr);
    return true;
  }
  std::size_t size() const { return size_; }
  const char *data() const { return data_; }
  unsigned char operator[](const std::size_t off) const {
    if (off >= size_) {
      return 255;
    }
    return (unsigned char)data_[off];
  }
  bool startswith(const char *prefix, size_t pl) const {
    if (pl >= size_) {
      return false;
    }
    return memcmp(data_, prefix, pl) == 0;
  }
  bool startswith(std::string_view sv) const {
    return startswith(sv.data(), sv.size());
  }
  bool indexswith(std::size_t offset, std::string_view sv) const {
    if (offset > size_) {
      return false;
    }
    return memcmp(data_ + offset, sv.data(), sv.size()) == 0;
  }

  template <typename T> const T *cast(size_t off) {
    if (off + sizeof(T) >= size_) {
      return nullptr;
    }
    return reinterpret_cast<const T *>(data_ + off);
  }

  memview subview(size_t off) {
    if (off >= size_) {
      return memview();
    }
    return memview(data_ + off, size_ - off);
  }

private:
  HANDLE FileHandle{nullfile_t};
  HANDLE FileMap{nullfile_t};
  char *data_{nullptr};
  std::size_t size_{0};
};

} // namespace planck

#endif