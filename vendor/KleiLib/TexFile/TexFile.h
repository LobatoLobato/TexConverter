#pragma once

#pragma region License
/*
Klei Studio is licensed under the MIT license.
Copyright © 2013 Matt Stevens

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma region License

#include <exception>
#include <string>
#include <cstdint>
#include <fstream>
#include <bitset>
#include <optional>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "BinaryTools/BinaryWriter.h"
#include "BinaryTools/BinaryReader.h"
#include "BinaryTools/Span.h"

namespace KleiLib
{
  class TexFile {
  public:
      struct InvalidTEXFileException : std::exception {
          explicit InvalidTEXFileException(const std::string& msg) : std::exception(msg.c_str()) { }
      };

      enum Platform : uint8_t {
          PC [[maybe_unused]] = 12,
          XBOX360 [[maybe_unused]] = 11,
          PS3 [[maybe_unused]] = 10,
          Unknown [[maybe_unused]] = 0
      };

      enum class PixelFormat : uint8_t {
          DXT1 = 0, DXT3 = 1, DXT5 = 2,
          ARGB = 4,
          Unknown [[maybe_unused]] = 7
      };

      enum TextureType : uint8_t {
          //[Description("1D")]
          OneD = 1,
          //[Description("2D")]
          TwoD [[maybe_unused]] = 2,
          //[Description("3D")]
          ThreeD [[maybe_unused]] = 3,
          //[Description("Cube Mapped")]
          Cubemap [[maybe_unused]] = 4
      };

      const char KTEXHeader[4] = {'K', 'T', 'E', 'X'};

      struct FileStruct {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
          struct HeaderStruct {
              uint32_t platform;
              uint32_t pixelformat;
              uint32_t texturetype;
              uint32_t nummips;
              uint32_t flags;
              uint32_t remainder;
          };
#pragma clang diagnostic pop

          HeaderStruct header{};
          std::vector<uint8_t> raw{};
      };

      FileStruct file;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
      struct Mipmap {
          uint16_t width{};
          uint16_t height{};
          uint16_t pitch{};
          uint32_t datasize{};
          std::vector<uint8_t> data;
      };
#pragma clang diagnostic pop

      TexFile() = default;

      explicit TexFile(const std::string& path);

      /* A little hacky but it gets the job done. */
  private:
      uint32_t oldremainder{};

  public:
      [[maybe_unused]] [[nodiscard]] bool IsPreCaveUpdate() const { return oldremainder == 262143; }

      [[maybe_unused]] [[nodiscard]] std::vector<Mipmap> GetMipmaps() const;

      [[maybe_unused]] [[nodiscard]] std::vector<Mipmap> GetMipmapsSummary() const;

      [[maybe_unused]] [[nodiscard]] Mipmap GetMainMipmap() const;

      [[maybe_unused]] [[nodiscard]] Mipmap GetMainMipmapSummary() const;

      void SaveFile(const std::string& path) const;
  };
}
