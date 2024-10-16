//
// Created by Lobato on 16/10/2024.
//
#include "TexConverter/Image.hpp"

#include <cstdint>
#include <string>
#include <cmath>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "private/gaussian_blur.hpp"

// t is a value that goes from 0 to 1 to interpolate in a C1 continuous way across uniformly sampled data points.
// when t is 0, this will return B.  When t is 1, this will return C.  Inbetween values will return an interpolation
// between B and C.  A and B are used to calculate slopes at the edges.
double CubicHermite(double A, double B, double C, double D, double t) {
  double a = -A / 2.0f + (3.0f * B) / 2.0f - (3.0f * C) / 2.0f + D / 2.0f;
  double b = A - (5.0f * B) / 2.0f + 2.0f * C - D / 2.0f;
  double c = -A / 2.0f + C / 2.0f;
  double d = B;

  return a * t * t * t + b * t * t + c * t + d;
}

namespace TexConverter
{
  template class Image<uint8_t>;

  template class Image<uint16_t>;

  template<typename ChannelT>
  Image<ChannelT>::Image(const std::string& path) {
    if constexpr (std::is_same_v<ChannelT, uint16_t>) {
      _data = stbi_load_16(path.c_str(), &_width, &_height, &_channels, 0);
    } else {
      _data = stbi_load(path.c_str(), &_width, &_height, &_channels, 0);
    }

    _data_len = _width * _height * _channels;
    _is_rgba = _channels == 4;
  }

  template<typename ChannelT>
  Image<ChannelT>::Image(int width, int height, int channels) : _width(width), _height(height), _channels(channels) {
    _data_len = width * height * channels;
    _data = new ChannelT[_data_len];
    _is_rgba = channels == 4;
  }

  template<typename ChannelT>
  Image<ChannelT>::Image(const Image& o)
    : _width(o._width), _height(o._height), _channels(o._channels), _is_rgba(o._is_rgba) {
    _data = new ChannelT[o._data_len];
    _data_len = o._data_len;
    std::copy(o._data, o._data + o._data_len, _data);
  }

  template<typename ChannelT>
  Image<ChannelT>::~Image() { stbi_image_free(_data); };

  template<typename ChannelT>
  Image<ChannelT>& Image<ChannelT>::flip(FlipType type) {
    auto* temp = new ChannelT[_data_len];
    auto stride = _width * _channels;

    for (size_t r = 0; r < _height; r++) {
      if (type == FlipType::Vertical) {
        auto src = &_data[r * stride], dst = &temp[(_height - r - 1) * stride];
        std::copy(src, src + stride, dst);
      } else if (type == FlipType::Horizontal) {
        for (size_t c = 0; c < stride; c += _channels) {
          auto src = &_data[r * stride + c];
          auto dst = &temp[r * stride + stride - c - _channels];
          std::copy(src, src + _channels, dst);
        }
      }
    }

    delete[] _data;
    _data = temp;

    return *this;
  }

  template<typename ChannelT>
  Image<ChannelT> Image<ChannelT>::resize(int width, int height, InterpolationMode mode) const {
    const Image* source_image = this;

    switch (mode) {
      case InterpolationMode::HighQualityBilinear:
      case InterpolationMode::HighQualityBicubic: {
        source_image = new Image(std::move(Image::prefilterGaussian(*this)));
      }
      default: break;
    }

    Image resized_image(width, height, _channels);

    for (int y = 0; y < height; ++y) {
      double v = double(y) / double(height);
      for (int x = 0; x < width; ++x) {
        double u = double(x) / double(width);

        resized_image.setPixel(x, y, source_image->sample(u, v, mode));
      }
    }

    if (source_image != this) { delete source_image; }

    return resized_image;
  }

  template<typename ChannelT>
  void Image<ChannelT>::write(std::string filename) {
    std::string ext = filename.substr(filename.size() - 3);

    if (ext == "bmp")
      stbi_write_bmp(filename.c_str(), _width, _height, _channels, _data);
    else if (ext == "jpg")
      stbi_write_jpg(filename.c_str(), _width, _height, _channels, _data, 90);
    else {
      if (ext != "png") {
        printf("Image format '%s' not supported, writing default png\n", ext.c_str());
        filename = filename.substr(0, filename.size() - 4) + std::string(".png");
      }
      stbi_write_png(filename.c_str(), _width, _height, _channels, _data, _channels * _width);
    }
  }

  template<typename ChannelT>
  Image<ChannelT>::PixelV4 Image<ChannelT>::sample(double u, double v, InterpolationMode mode) const {
    double denorm_x = (u * _width), denorm_y = (v * _height);
    switch (mode) {
      case InterpolationMode::NearestNeighbor: return Image::sampleNearest(*this, denorm_x, denorm_y);
      case InterpolationMode::HighQualityBilinear:
      case InterpolationMode::Bilinear: return Image::sampleBilinear(*this, denorm_x, denorm_y);
      case InterpolationMode::HighQualityBicubic:
      case InterpolationMode::Bicubic: return Image::sampleBicubic(*this, denorm_x, denorm_y);
      case InterpolationMode::Invalid:
      default:return {};
    }
  }

  template<typename ChannelT>
  Image<ChannelT>::PixelV4 Image<ChannelT>::sampleNearest(const Image& image, double x, double y) {
    return image.pixelAt(std::floor(x + 1), std::floor(y + 1));
  }

  template<typename ChannelT>
  Image<ChannelT>::PixelV4 Image<ChannelT>::sampleBilinear(const Image& image, double x, double y) {
    int xint = std::floor(x + 0.5), yint = std::floor(y + 0.5);
    double xfract = (x + 0.5) - xint, yfract = (y + 0.5) - yint;
    double x_opp = 1 - xfract, y_opp = 1 - yfract;

    const auto bl_interpolate_values = [&](double tl, double tr, double bl, double br) {
      return (tl * x_opp + tr * xfract) * y_opp + (bl * x_opp + br * xfract) * yfract;
    };


    auto pxtl = image.pixelAt(xint + 0, yint + 0);
    auto pxtr = image.pixelAt(xint + 1, yint + 0);
    auto pxbl = image.pixelAt(xint + 0, yint + 1);
    auto pxbr = image.pixelAt(xint + 1, yint + 1);
    ChannelT alphamean = (pxtl.a + pxtr.a + pxbl.a + pxbr.a) / 4;

    PixelV4 sample;
    for (int channel = 0; channel < 3; channel++) {
      ChannelT tl = pxtl[channel], tr = pxtr[channel], bl = pxbl[channel], br = pxbr[channel];
      double value = bl_interpolate_values(tl * pxtl.a, tr * pxtr.a, bl * pxbl.a, br * pxbr.a);
      sample[channel] = (alphamean > 0) ? ChannelT(value / alphamean) : 0;
    }
    if (image._is_rgba) { sample.a = ChannelT(bl_interpolate_values(pxtl.a, pxtr.a, pxbl.a, pxbr.a)); }

    return sample;

  }

  template<typename ChannelT>
  Image<ChannelT>::PixelV4 Image<ChannelT>::sampleBicubic(const Image& image, double x, double y) {
    // calculate coordinates -> also need to offset by half a pixel to keep image from shifting down and left half a pixel
    int xint = std::floor(x + 0.5), yint = std::floor(y + 0.5);
    double xfract = (x + 0.5) - xint, yfract = (y + 0.5) - yint;

    double alphamean = 0;
    // 1st row
    auto p00 = image.pixelAt(xint - 1, yint - 1);
    auto p10 = image.pixelAt(xint + 0, yint - 1);
    auto p20 = image.pixelAt(xint + 1, yint - 1);
    auto p30 = image.pixelAt(xint + 2, yint - 1);
    alphamean += p00.a + p10.a + p20.a + p30.a;

    // 2nd row
    auto p01 = image.pixelAt(xint - 1, yint + 0);
    auto p11 = image.pixelAt(xint + 0, yint + 0);
    auto p21 = image.pixelAt(xint + 1, yint + 0);
    auto p31 = image.pixelAt(xint + 2, yint + 0);
    alphamean += p01.a + p11.a + p21.a + p31.a;

    // 3rd row
    auto p02 = image.pixelAt(xint - 1, yint + 1);
    auto p12 = image.pixelAt(xint + 0, yint + 1);
    auto p22 = image.pixelAt(xint + 1, yint + 1);
    auto p32 = image.pixelAt(xint + 2, yint + 1);
    alphamean += p02.a + p12.a + p22.a + p32.a;

    // 4th row
    auto p03 = image.pixelAt(xint - 1, yint + 2);
    auto p13 = image.pixelAt(xint + 0, yint + 2);
    auto p23 = image.pixelAt(xint + 1, yint + 2);
    auto p33 = image.pixelAt(xint + 2, yint + 2);
    alphamean += p03.a + p13.a + p23.a + p33.a;

    alphamean /= 16;

    PixelV4 sample;
    for (int c = 0; c < 3; ++c) {
      double col0 = CubicHermite(p00[c] * p00.a, p10[c] * p10.a, p20[c] * p20.a, p30[c] * p30.a, xfract);
      double col1 = CubicHermite(p01[c] * p01.a, p11[c] * p11.a, p21[c] * p21.a, p31[c] * p31.a, xfract);
      double col2 = CubicHermite(p02[c] * p02.a, p12[c] * p12.a, p22[c] * p22.a, p32[c] * p32.a, xfract);
      double col3 = CubicHermite(p03[c] * p03.a, p13[c] * p13.a, p23[c] * p23.a, p33[c] * p33.a, xfract);
      double value = CubicHermite(col0, col1, col2, col3, yfract) / alphamean;

      sample[c] = ChannelT(std::max(0.0, std::min(value, 255.0)));
    }

    if (image._is_rgba) {
      double col0 = CubicHermite(p00.a, p10.a, p20.a, p30.a, xfract);
      double col1 = CubicHermite(p01.a, p11.a, p21.a, p31.a, xfract);
      double col2 = CubicHermite(p02.a, p12.a, p22.a, p32.a, xfract);
      double col3 = CubicHermite(p03.a, p13.a, p23.a, p33.a, xfract);
      double value = CubicHermite(col0, col1, col2, col3, yfract);

      sample.a = ChannelT(std::max(0.0, std::min(value, 255.0)));
    }

    return sample;
  }

  template<typename ChannelT>
  Image<ChannelT> Image<ChannelT>::prefilterGaussian(Image src) {
    Image dest(src._width, src._height, src._channels);
    fast_gaussian_blur(src._data, dest._data, src._width, src._height, src._channels, 1, 1, kMirror);
    return dest;
  }

  template<typename ChannelT>
  size_t Image<ChannelT>::coordsToIndex(int x, int y) const {
    return y * _width * _channels + x * _channels;
  }

  template<typename ChannelT>
  Image<ChannelT>::PixelV4 Image<ChannelT>::pixelAt(int x, int y) const {
    return {&_data[std::min(coordsToIndex(x, y), _data_len)], _channels};
  }

  template<typename ChannelT>
  void Image<ChannelT>::setPixel(int x, int y, const PixelV4& pixel) const {
    for (int channel = 0; channel < _channels; channel++) {
      _data[coordsToIndex(x, y) + channel] = pixel[channel];
    }
  }

}