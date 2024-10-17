//
// Created by Lobato on 17/10/2024.
//
#include "KleiLib/Mipmap.h"

#include <format>

namespace KleiLib
{
  Mipmap::Mipmap(const Image::Image8& inputImage, KleiLib::Mipmap::PixelFormat pixelFormat, bool preMultiplyAlpha)
  : width(inputImage.width()), height(inputImage.height()), datasize(height * width * inputImage.channels()), pitch(0) {
    std::vector<uint8_t> rgba(width * height * inputImage.channels());
    int channels = inputImage.channels();

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        auto c = inputImage.pixelAt(x, y);

        if (preMultiplyAlpha) {
          double alphamod = double(c.a) / 255.0;

          c.r = static_cast<uint8_t>(c.r * alphamod);
          c.g = static_cast<uint8_t>(c.g * alphamod);
          c.b = static_cast<uint8_t>(c.b * alphamod);
        }

        rgba[y * width * channels + x * channels + 0] = (c.r);
        rgba[y * width * channels + x * channels + 1] = (c.g);
        rgba[y * width * channels + x * channels + 2] = (c.b);
        rgba[y * width * channels + x * channels + 3] = (c.a);
      }
    }

    int flags;
    switch (pixelFormat) {
      case PixelFormat::DXT1: flags = squish::kDxt1; break;
      case PixelFormat::DXT3: flags = squish::kDxt3; break;
      case PixelFormat::DXT5: flags = squish::kDxt5; break;
      case PixelFormat::ARGB: data = std::move(rgba); return;
      default: throw InvalidPixelFormatException(pixelFormat);
    }

    data.resize(squish::GetStorageRequirements(width, height, flags));
    squish::CompressImage(rgba.data(), width, height, width * channels, data.data(), flags);
  }

  Mipmap::InvalidPixelFormatException::InvalidPixelFormatException(Mipmap::PixelFormat v)
  : InvalidPixelFormatException(uint32_t(v)) {}
  Mipmap::InvalidPixelFormatException::InvalidPixelFormatException(uint32_t v)
  : std::exception(std::format("Unknown pixel format: {}", v).c_str()) {}

}// namespace KleiLib