#include "TexFile.h"

namespace KleiLib
{

  TexFile::TexFile(const std::string& path) {
    BinaryReader reader(path);

    if (reader.ReadFixedLengthString(4) != KTEXHeader)
      throw InvalidTEXFileException("The first 4 bytes do not match 'KTEX'.");

    auto header = reader.ReadUint32();

    std::cout << std::bitset<32>(header) << "\n";

    file.header.platform = header & 15;
    file.header.pixelformat = (header >> 4) & 31;
    file.header.texturetype = (header >> 9) & 15;
    file.header.nummips = (header >> 13) & 31;
    file.header.flags = (header >> 18) & 3;
    file.header.remainder = (header >> 20) & 4095;

    // Just a little hack for pre cave updates, can remove later.
    oldremainder = (header >> 14) & 262143;

    size_t remainderlen = reader.Length() - reader.Position();

    file.raw = reader.ReadBytes(remainderlen);
  }

  void TexFile::SaveFile(const std::string& path) const {
    BinaryWriter writer(path);
    writer.Write(KTEXHeader);

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

  [[maybe_unused]] std::vector<TexFile::Mipmap> TexFile::GetMipmaps() const {
    BinaryReader reader(file.raw);
    std::vector<Mipmap> mipmapArray;
    mipmapArray.reserve(file.header.nummips);


    for (int i = 0; i < file.header.nummips; i++) {
      mipmapArray[i] = Mipmap();
      mipmapArray[i].width = reader.ReadUint16();
      mipmapArray[i].height = reader.ReadUint16();
      mipmapArray[i].pitch = reader.ReadUint16();
      mipmapArray[i].datasize = reader.ReadUint32();
    }

    for (auto& mipmap : mipmapArray) {
      mipmap.data = reader.ReadBytes((int) mipmap.datasize);
    }

    return mipmapArray;
  }

  [[maybe_unused]] std::vector<TexFile::Mipmap> TexFile::GetMipmapsSummary() const {
    BinaryReader reader(file.raw);
    std::vector<Mipmap> mipmapArray;
    mipmapArray.reserve(file.header.nummips);


    for (int i = 0; i < file.header.nummips; i++) {
      mipmapArray[i] = Mipmap();
      mipmapArray[i].width = reader.ReadUint16();
      mipmapArray[i].height = reader.ReadUint16();
      mipmapArray[i].pitch = reader.ReadUint16();
      mipmapArray[i].datasize = reader.ReadUint32();
    }

    return mipmapArray;
  }

  [[maybe_unused]] TexFile::Mipmap TexFile::GetMainMipmap() const {
    BinaryReader reader(file.raw);
    Mipmap mipmap;

    mipmap.width = reader.ReadUint16();
    mipmap.height = reader.ReadUint16();
    mipmap.pitch = reader.ReadUint16();
    mipmap.datasize = reader.ReadUint32();

    reader.SeekCur((static_cast<size_t>(file.header.nummips) - 1) * 10);

    mipmap.data = reader.ReadBytes((int) mipmap.datasize);

    return mipmap;
  }

  [[maybe_unused]] TexFile::Mipmap TexFile::GetMainMipmapSummary() const {
    BinaryReader reader(file.raw);
    Mipmap mipmap = Mipmap();

    mipmap.width = reader.ReadUint16();
    mipmap.height = reader.ReadUint16();
    mipmap.pitch = reader.ReadUint16();
    mipmap.datasize = reader.ReadUint32();


    return mipmap;
  }

}