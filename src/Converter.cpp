#include "TexConverter/Converter.hpp"

#include <squish/squish.h>

namespace TexConverter
{
  struct Mipmap {
      uint16_t width{};
      uint16_t height{};
      uint16_t pitch{};
      std::vector<uint8_t> ARGBData{};

      Mipmap() = default;

      Mipmap(uint16_t w, uint16_t h, uint16_t p, std::vector<uint8_t> d) {
        width = w;
        height = h;
        pitch = p;
        ARGBData = std::move(d);
      }
  };

  Mipmap GenerateMipmap(const Image8& inputImage, PixelFormat pixelFormat, bool preMultiplyAlpha) {
    int width = inputImage.width(), height = inputImage.height();
    int channels = inputImage.channels(), pitch = width * channels;

    std::vector<uint8_t> rgba;
    rgba.resize(width * height * channels);

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        auto c = inputImage.pixelAt(x, y);

        if (preMultiplyAlpha) {
          double alphamod = (double) c.a / 255.0f; // Normalize.

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

    Mipmap mipmap;
    mipmap.width = uint16_t(width);
    mipmap.height = uint16_t(height);
    mipmap.pitch = 0;

    switch (pixelFormat) {
      case PixelFormat::DXT1: mipmap.ARGBData.resize(squish::GetStorageRequirements(width, height, squish::kDxt1));
        squish::CompressImage(rgba.data(), width, height, pitch, mipmap.ARGBData.data(), squish::kDxt1);
        break;
      case PixelFormat::DXT3: mipmap.ARGBData.resize(squish::GetStorageRequirements(width, height, squish::kDxt3));
        squish::CompressImage(rgba.data(), width, height, pitch, mipmap.ARGBData.data(), squish::kDxt3);
        break;
      case PixelFormat::DXT5: mipmap.ARGBData.resize(squish::GetStorageRequirements(width, height, squish::kDxt5));
        squish::CompressImage(rgba.data(), width, height, pitch, mipmap.ARGBData.data(), squish::kDxt5);
        break;
      case PixelFormat::ARGB: mipmap.ARGBData = std::move(rgba);
        break;
      default:break;
    }

    return mipmap;
  }

  Mipmap GenerateMipmap(
    const Image8& inputImage,
    PixelFormat pixelFormat,
    int width,
    int height,
    MipmapFilter mode,
    bool preMultiplyAlpha
  ) {
    return GenerateMipmap(inputImage.resize(width, height, mode), pixelFormat, preMultiplyAlpha);
  }

  void convertImageToTex(
    const std::string& inputFile,
    const std::string& outputFile,
    PixelFormat pixelFormat,
    MipmapFilter interpolationMode,
    TextureType textureType,
    bool generateMipmaps,
    bool preMultiplyAlpha
  ) {
    Image8 inputImage(inputFile);
    inputImage.flip(TexConverter::FlipType::Vertical);

    std::vector<Mipmap> mipmaps = {GenerateMipmap(inputImage, pixelFormat, preMultiplyAlpha)};

    if (generateMipmaps) {
      auto width = inputImage.width();
      auto height = inputImage.height();

      while (std::max(width, height) > 1) {
        width = std::max(1, width >> 1);
        height = std::max(1, height >> 1);


        mipmaps.push_back(GenerateMipmap(inputImage,
          pixelFormat,
          width,
          height,
          interpolationMode,
          preMultiplyAlpha));
      }
    }

    KleiLib::TexFile outputTexFile;

    outputTexFile.file.header.platform = 0;
    outputTexFile.file.header.pixelformat = (uint16_t) pixelFormat;
    outputTexFile.file.header.texturetype = (uint16_t) textureType;
    outputTexFile.file.header.nummips = (uint16_t) mipmaps.size();
    outputTexFile.file.header.flags = 0;

    BinaryWriter writer;

    for (const Mipmap& mip : mipmaps) {
      writer.Write(mip.width);
      writer.Write(mip.height);
      writer.Write(mip.pitch);
      writer.Write((uint32_t) mip.ARGBData.size());
    }

    for (const Mipmap& mip : mipmaps) {
      writer.WriteSpan<uint8_t>(mip.ARGBData);
    }

    outputTexFile.file.raw = writer.GetBuffer();

    outputTexFile.SaveFile(outputFile);
  }


}