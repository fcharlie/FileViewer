//////// GIT pack index and other files.
#include <endian.hpp>
#include <strcat.hpp>
#include "inquisitive.hpp"

namespace inquisitive {
// todo resolve git index pack and midx files
#pragma pack(1)
struct git_pack_header_t {
  uint8_t signature[4]; /// P A C K
  uint32_t version;     // BE
  uint32_t objsize;     // BE
};

struct git_index_header_t {
  uint8_t siganture[4];
  uint32_t version;
  uint32_t fanout[256];
};

struct git_index3_header_t {
  uint8_t signature[4]; // 0xFF, 0x74, 0x4F, 0x63
  uint32_t version;
  uint32_t hslength;
  uint32_t packobjects;
  uint32_t objectformats; // 2
};

struct git_midx_header_t {
  uint8_t siganture[4]; // M I D X
  uint8_t version;
  uint8_t oidversion; // 1 SHA1
  uint8_t chunks;
  uint8_t basemidxsize; // 0
  uint32_t packfiles;   //
};
#pragma pack()
// https://github.com/git/git/blob/master/Documentation/technical/pack-format.txt
status_t inquisitive_gitbinary(memview mv, inquisitive_result_t &ir) {
  constexpr const byte_t packMagic[] = {'P', 'A', 'C', 'K'};
  constexpr const byte_t midxMagic[] = {'M', 'I', 'D', 'X'};
  constexpr const byte_t indexMagic[] = {0xFF, 0x74, 0x4F, 0x63};
  if (mv.startswith(packMagic)) {
    auto hd = mv.cast<git_pack_header_t>(0);
    if (hd == nullptr) {
      return None;
    }
    auto name = planck::StrCat(L"Git pack file, version ",
                               planck::resolvebe(hd->version), L", objects ",
                               planck::resolvebe(hd->objsize));
    ir.assign(std::move(name), types::gitpack);
    return Found;
  }
  if (mv.startswith(indexMagic)) {
    auto hd = mv.cast<git_index_header_t>(0);
    if (hd == nullptr) {
      return None;
    }
    std::wstring name;
    auto ver = planck::resolvebe(hd->version);
    switch (ver) {
    case 2:
      name = planck::StrCat(L"Git pack indexs file, version ", ver,
                            L", total objects ",
                            planck::resolvebe(hd->fanout[255]));
      break;
    case 3: {
      auto hd3 = mv.cast<git_index3_header_t>(0);
      name = planck::StrCat(L"Git pack indexs file, version ", ver,
                            L", total objects ",
                            planck::resolvebe(hd3->packobjects));
    } break;
    default:
      name = planck::StrCat(L"Git pack indexs file, version ", ver);
      break;
    };

    ir.assign(std::move(name), types::gitpkindex);
    return Found;
  }
  if (mv.startswith(midxMagic)) {
    auto hd = mv.cast<git_midx_header_t>(0);
    if (hd == nullptr) {
      return None;
    }
    auto name = planck::StrCat(
        L"Git multi-pack-index, version ", (int)hd->version, L", oid version ",
        (int)hd->oidversion, L", chunks ", (int)hd->chunks, L", pack files ",
        planck::resolvebe(hd->packfiles));
    ir.assign(std::move(name), types::gitpack);
    return Found;
  }

  return None;
}

} // namespace inquisitive
