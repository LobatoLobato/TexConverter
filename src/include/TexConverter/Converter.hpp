//
// Created by Lobato on 14/10/2024.
//

#ifndef TEXCONVERTER_CONVERTER_HPP
#define TEXCONVERTER_CONVERTER_HPP

#include "Image/Image.hpp"
#include <KleiLib/TexFile.h>
#include <cstdint>
#include <string>
#include <vector>


namespace TexConverter
{
  using PixelFormat = KleiLib::Mipmap::PixelFormat;
  using TextureType = KleiLib::TexFile::TextureType;
  using MipmapFilter = Image::InterpolationMode;

  void convertImageToTex(
    Image::Image8 image, const std::string& outputFile, PixelFormat pixelFormat = PixelFormat::DXT5,
    MipmapFilter interpolationMode = MipmapFilter::Default, TextureType textureType = TextureType::OneD,
    bool generateMipmaps = false, bool preMultiplyAlpha = false
  );

  void convertImageToTex(
    const std::string& inputFile, const std::string& outputFile, PixelFormat pixelFormat = PixelFormat::DXT5,
    MipmapFilter interpolationMode = MipmapFilter::Default, TextureType textureType = TextureType::OneD,
    bool generateMipmaps = false, bool preMultiplyAlpha = false
  );

  Image::Image8 convertTexToImage(const std::string& inputFile);

  void convertTexToImage(const std::string& inputFile, const std::string& outputFile);

}// namespace TexConverter

#endif// TEXCONVERTER_CONVERTER_HPP