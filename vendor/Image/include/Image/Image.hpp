//
// Created by Lobato on 16/10/2024.
//

#ifndef TEXCONVERTER_IMAGE_HPP
#define TEXCONVERTER_IMAGE_HPP

#include <cstdint>
#include <array>
#include <string>

namespace Image
{
  enum class FlipType {
      Horizontal, Vertical
  };

  enum class InterpolationMode : int8_t {
      Invalid = -1,
      Default [[maybe_unused]] = 2,
      NearestNeighbor = 1,
      Bilinear = 2,
      Bicubic = 3,
      HighQualityBilinear = 4,
      HighQualityBicubic = 5,
      Low [[maybe_unused]] = 2,
      High [[maybe_unused]] = 5,
  };

  template<typename ChannelT>
  struct Image {

      struct PixelV4 {
          PixelV4() = default;

          PixelV4(const ChannelT* pixeldata, int n_channels) {
            for (int chidx = 0; chidx < n_channels; chidx++) { channels[chidx] = pixeldata[chidx]; }
          }

          PixelV4(ChannelT r, ChannelT g, ChannelT b, ChannelT a) : channels({r, g, b, a}) { }

          ChannelT& operator[](int index) { return channels[index]; };

          const ChannelT& operator[](int index) const { return channels[index]; };

      private:
          std::array<ChannelT, 4> channels = {0, 0, 0, 1};
      public:
          ChannelT& r = channels[0], & g = channels[1], & b = channels[2], & a = channels[3];
      };

      Image() : _data(new ChannelT[0]), _data_len(0), _is_rgba(false) { }

      explicit Image(const std::string& path);

      Image(int width, int height, int channels);

      Image(ChannelT* data, int width, int height, int channels);

      Image(const Image& o);

      ~Image();

      Image toRGB() const;
      [[maybe_unused]] Image toRGBA(bool white_to_transparent = false, float tolerance = 0) const;

      Image& flip(FlipType type);

      Image resize(int width, int height, InterpolationMode mode) const;

      void write(std::string filename);

      [[nodiscard]] int width() const { return _width; }

      [[nodiscard]] int height() const { return _height; }

      [[maybe_unused]] [[nodiscard]] int channels() const { return _channels; }

      [[nodiscard]] PixelV4 pixelAt(int x, int y) const;

      void setPixel(int x, int y, const PixelV4& pixel) const;


  private:

      [[nodiscard]] PixelV4 sample(double u, double v, InterpolationMode mode) const;

      static PixelV4 sampleNearest(const Image& image, double x, double y);

      static PixelV4 sampleBilinear(const Image& image, double x, double y);

      static PixelV4 sampleBicubic(const Image& image, double x, double y);

      static Image prefilterGaussian(Image src);

      [[nodiscard]] size_t coordsToIndex(int x, int y) const;

  private:
      ChannelT* _data;
      size_t _data_len;

      int _width{}, _height{}, _channels{};

      bool _is_rgba;
  };


  using Image8 [[maybe_unused]] = Image<uint8_t>;
  using Image16 [[maybe_unused]] = Image<uint16_t>;
}
#endif //TEXCONVERTER_IMAGE_HPP
