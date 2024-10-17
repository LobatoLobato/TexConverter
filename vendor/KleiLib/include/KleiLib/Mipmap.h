//
// Created by Lobato on 17/10/2024.
//

#ifndef KLEILIB_MIPMAP_H
#define KLEILIB_MIPMAP_H

#include <cstdint>
#include <exception>
#include <vector>

#include <Image/Image.hpp>
#include <squish/squish.h>

namespace KleiLib
{
  struct Mipmap {
    using Filter = Image::InterpolationMode;

    enum class PixelFormat : uint8_t {
      DXT1 = 0, DXT3 = 1, DXT5 = 2,
      ARGB = 4,
      Unknown [[maybe_unused]] = 7
    };

    struct InvalidPixelFormatException: std::exception {
      explicit InvalidPixelFormatException(PixelFormat v);
      explicit InvalidPixelFormatException(uint32_t v);
    };

    uint16_t width{};
    uint16_t height{};
    uint16_t pitch{};
    uint32_t datasize{};
    std::vector<uint8_t> data;

    Mipmap() = default;

    Mipmap(uint16_t w, uint16_t h, uint16_t p, std::vector<uint8_t> d)
    : width(w), height(h), pitch(p), data(std::move(d)) {}

    Mipmap(const Image::Image8& inputImage, PixelFormat pixelFormat, bool preMultiplyAlpha);

    Mipmap(
      const Image::Image8& inputImage,
      PixelFormat pixelFormat,
      int width,
      int height,
      Mipmap::Filter mode,
      bool preMultiplyAlpha
    )
    : Mipmap(inputImage.resize(width, height, mode), pixelFormat, preMultiplyAlpha) {}
  };
}// namespace KleiLib

#endif//KLEILIB_MIPMAP_H
