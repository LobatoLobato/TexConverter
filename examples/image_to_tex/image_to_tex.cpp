//
// Created by Lobato on 16/10/2024.
//
#include <iostream>
#include <string>
#include <TexConverter/Converter.hpp>

int main() {
  // Input can be any image type supported by stb_image
  std::string inputImagePath = "wurt.png";
  std::string outputTexPath = "wurt.tex";
  auto pixelFormat = TexConverter::PixelFormat::DXT5;
  auto interpolationMode = TexConverter::MipmapFilter::Default;
  auto textureType = TexConverter::TextureType::OneD;
  bool generateMipmaps = false;
  bool preMultiplyAlpha = false;

  {
    std::cout << "Converting " << inputImagePath << "...\n";
    TexConverter::convertImageToTex(
      inputImagePath,
      outputTexPath,
      pixelFormat,
      interpolationMode,
      textureType,
      generateMipmaps,
      preMultiplyAlpha
    );
    std::cout << "Converted " << inputImagePath << ".\n"
              << "Saved to " << outputTexPath << ".\n";
  }
}