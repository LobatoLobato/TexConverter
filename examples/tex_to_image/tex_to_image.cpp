//
// Created by Lobato on 17/10/2024.
//
#include <TexConverter/Converter.hpp>
#include <iostream>
#include <string>

int main() {
  // Output can be any image type supported by stb_image
  std::string inputImagePath = "wurt.tex";
  std::string outputTexPath = "wurt.png";

  std::cout << "Converting " << inputImagePath << "...\n";

  TexConverter::convertTexToImage(inputImagePath, outputTexPath);

  std::cout << "Converted " << inputImagePath << ".\n"
            << "Saved to " << outputTexPath << ".\n";
}