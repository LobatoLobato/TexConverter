//
// Created by Lobato on 14/10/2024.
//

#include "KleiLib/TexFile.h"

#include <filesystem>

#include <BinaryTools/BinaryReader.h>
#include <BinaryTools/BinaryWriter.h>
#include <squish/squish.h>

#include <Image/Image.hpp>

namespace KleiLib
{
  TexFile::TexFile(const std::string& path) {
    BinaryReader reader(path);

    if (reader.ReadFixedLengthString(4) != KTEXHeader) {
      throw InvalidTexFileException("The first 4 bytes do not match 'KTEX'.");
    }

    auto header = reader.ReadUint32();

    file.header.platform = header & 15;
    file.header.pixelformat = (header >> 4) & 31;
    file.header.texturetype = (header >> 9) & 15;
    file.header.nummips = (header >> 13) & 31;
    file.header.flags = (header >> 18) & 3;
    file.header.remainder = (header >> 20) & 4095;

    file.raw = reader.ReadBytes(reader.Length() - reader.Position());
  }

  TexFile::TexFile(
    Platform platform,
    Mipmap::PixelFormat pixelformat,
    TextureType texturetype,
    uint32_t flags,
    const std::vector<Mipmap>& mipmaps
  ) {
    BinaryWriter writer;

    for (const Mipmap& mip : mipmaps) {
      writer.Write(mip.width);
      writer.Write(mip.height);
      writer.Write(mip.pitch);
      writer.Write((uint32_t)mip.data.size());
    }

    for (const Mipmap& mip : mipmaps) { writer.WriteSpan<uint8_t>(mip.data); }

    file.header = {
      .platform = uint32_t(platform),
      .pixelformat = uint32_t(pixelformat),
      .texturetype = uint32_t(texturetype),
      .nummips = uint32_t(mipmaps.size()),
      .flags = flags,
    };

    file.raw = writer.GetBuffer();
  }

  Mipmap TexFile::decompress() const {
    BinaryReader reader(file.raw);
    Mipmap mipmap;

    mipmap.width = reader.ReadUint16();
    mipmap.height = reader.ReadUint16();
    mipmap.pitch = reader.ReadUint16();
    mipmap.datasize = reader.ReadUint32();

    reader.SeekCur((static_cast<size_t>(file.header.nummips) - 1) * 10);

    mipmap.data = std::move(reader.ReadBytes((int)mipmap.datasize));

    if (Mipmap::PixelFormat(file.header.pixelformat) == Mipmap::PixelFormat::ARGB) { return mipmap; }

    int flags, pitch = mipmap.width * TexChannels;

    switch (Mipmap::PixelFormat(file.header.pixelformat)) {
      case Mipmap::PixelFormat::DXT1: flags = squish::kDxt1; break;
      case Mipmap::PixelFormat::DXT3: flags = squish::kDxt3; break;
      case Mipmap::PixelFormat::DXT5: flags = squish::kDxt5; break;
      default: throw Mipmap::InvalidPixelFormatException(file.header.pixelformat);
    }

    std::vector<uint8_t> argbData(mipmap.width * mipmap.height * TexChannels);
    squish::DecompressImage(argbData.data(), mipmap.width, mipmap.height, pitch, mipmap.data.data(), flags);
    mipmap.data = std::move(argbData);

    return mipmap;
  }

  void TexFile::writeToFile(const std::string& path) const {
    BinaryWriter writer(path);

    writer.WriteFixedLengthString(KTEXHeader);

    uint32_t header = 0;

    header |= 4095;
    header <<= 2;
    header |= file.header.flags;
    header <<= 5;
    header |= file.header.nummips;
    header <<= 4;
    header |= file.header.texturetype;
    header <<= 5;
    header |= file.header.pixelformat;
    header <<= 4;
    header |= file.header.platform;

    writer.Write(header);
    writer.Write(file.raw);
  }

}// namespace KleiLib