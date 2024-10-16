//
// Created by Lobato on 14/10/2024.
//

#ifndef TEXCONVERTER_CONVERTER_HPP
#define TEXCONVERTER_CONVERTER_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <KleiLib/TexFile.h>
#include "TexConverter/Image.hpp"


namespace TexConverter
{
  using PixelFormat = KleiLib::TexFile::PixelFormat;
  using TextureType = KleiLib::TexFile::TextureType;
  using MipmapFilter = TexConverter::InterpolationMode;

  void convertImageToTex(
    const std::string& inputFile, const std::string& outputFile,
    PixelFormat pixelFormat = PixelFormat::DXT5,
    MipmapFilter interpolationMode = MipmapFilter::Default,
    TextureType textureType = TextureType::OneD,
    bool generateMipmaps = false, bool preMultiplyAlpha = false
  );
}
#endif // TEXCONVERTER_CONVERTER_HPP