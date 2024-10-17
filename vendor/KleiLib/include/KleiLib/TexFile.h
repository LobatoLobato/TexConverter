//
// Created by Lobato on 14/10/2024.
//

#ifndef KLEILIB_TEXFILE_HPP
#define KLEILIB_TEXFILE_HPP

#include <cstdint>
#include <exception>
#include <string>
#include <vector>

#include "KleiLib/Mipmap.h"

namespace KleiLib
{
  class TexFile {
  public:
    using InvalidTexFileException = std::exception;

    enum Platform : uint8_t {
      PC [[maybe_unused]] = 12,
      XBOX360 [[maybe_unused]] = 11,
      PS3 [[maybe_unused]] = 10,
      Unknown [[maybe_unused]] = 0
    };

    enum TextureType : uint8_t {
      OneD [[maybe_unused]] = 1,
      TwoD [[maybe_unused]] = 2,
      ThreeD [[maybe_unused]] = 3,
      Cubemap [[maybe_unused]] = 4
    };

    static inline const char* KTEXHeader = "KTEX";

    static const uint8_t TexChannels = 4;

    TexFile() = default;

    explicit TexFile(const std::string& path);

    TexFile(
      Platform platform,
      Mipmap::PixelFormat pixelformat,
      TextureType texturetype,
      uint32_t flags,
      const std::vector<Mipmap>& mipmaps
    );

    void writeToFile(const std::string& path) const;

    [[nodiscard]] Mipmap decompress() const;

  private:
    struct FileStruct {
      struct HeaderStruct {
        uint32_t platform;
        uint32_t pixelformat;
        uint32_t texturetype;
        uint32_t nummips;
        uint32_t flags;
        [[maybe_unused]] uint32_t remainder;
      };

      HeaderStruct header{};
      std::vector<uint8_t> raw{};
    } file;
  };

}// namespace KleiLib

#endif