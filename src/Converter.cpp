#include "TexConverter/Converter.hpp"

#include <BinaryTools/BinaryReader.h>
#include <KleiLib/Mipmap.h>

namespace TexConverter
{
  using Mipmap = KleiLib::Mipmap;

  void convertImageToTex(
    Image::Image8 image,
    const std::string& outputFile,
    PixelFormat pixelFormat,
    MipmapFilter interpolationMode,
    TextureType textureType,
    bool generateMipmaps,
    bool preMultiplyAlpha
  ) {
    image.flip(Image::FlipType::Vertical);

    std::vector<Mipmap> mipmaps = {{image, pixelFormat, preMultiplyAlpha}};

    if (generateMipmaps) {
      auto width = image.width();
      auto height = image.height();

      while (std::max(width, height) > 1) {
        width = std::max(1, width >> 1);
        height = std::max(1, height >> 1);

        mipmaps.emplace_back(image, pixelFormat, width, height, interpolationMode, preMultiplyAlpha);
      }
    }

    using TexFile = KleiLib::TexFile;
    TexFile outputTexFile(TexFile::Platform::Unknown, pixelFormat, textureType, 0, mipmaps);
    outputTexFile.writeToFile(outputFile);
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
    return convertImageToTex(
      Image::Image8{inputFile},
      outputFile,
      pixelFormat,
      interpolationMode,
      textureType,
      generateMipmaps,
      preMultiplyAlpha
    );
  }

  void convertTexToImage(const std::string& inputFile, const std::string& outputFile) {
    KleiLib::TexFile tex(inputFile);

    auto mipmap = tex.decompress();
    BinaryReader imgReader(mipmap.data);

    Image::Image8 image((int)mipmap.width, (int)mipmap.height, KleiLib::TexFile::TexChannels);

    for (int y = 0; y < mipmap.height; y++) {
      for (int x = 0; x < mipmap.width; x++) {
        uint8_t r = imgReader.ReadUint8();
        uint8_t g = imgReader.ReadUint8();
        uint8_t b = imgReader.ReadUint8();
        uint8_t a = imgReader.ReadUint8();
        image.setPixel(x, y, Image::Image8::PixelV4(r, g, b, a));
      }
    }

    image.flip(Image::FlipType::Vertical);

    image.write(outputFile);
  }
}// namespace TexConverter