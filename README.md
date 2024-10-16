# Summary
TexConverter is a c++ library for conversion between images and Klei's .tex files

Based on [Klei Studio](https://github.com/handsomematt/dont-starve-tools)

# Adding to a cmake project
>## Method 1: Cloning
>```sh
>$ cd path/to/project
>$ git clone https://github.com/LobatoLobato/TexConverter
>```
>```cmake
>add_subdirectory(TexConverter)
>target_link_libraries(my_target PRIVATE TexConverter)
>```
>## Method 2: FetchContent
>```cmake
>FetchContent_Declare(
>  TexConverter
>  GIT_REPOSITORY https://github.com/LobatoLobato/TexConverter
>)
>
>FetchContent_MakeAvailable(TexConverter)
>
>target_link_libraries(my_target PRIVATE TexConverter)
>```

# Examples

### `Image to tex conversion`
```c++
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
```
# Todo
  - Tex to image conversion
  - Implement Gdiplus-like HighQualityBilinear and HighQualityBicubic image interpolators

# Contributing
If you would like to contribute bug fixes and the likes, just make a pull request.

# Copyright and license
Copyright 2024 Felipe Ribeiro Lobato under [the MIT license](LICENSE).