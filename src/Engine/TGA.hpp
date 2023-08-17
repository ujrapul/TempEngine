#pragma once

#include "Logger.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

namespace Temp::TGA
{
#pragma pack(push, 1) // Ensure the structure is packed with no padding
  struct Header
  {
    char idLength;
    char colorMapType;
    char imageType;
    short int colorMapStart;
    short int colorMapLength;
    char colorMapDepth;
    short int xOrigin;
    short int yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
  };
#pragma pack(pop)

  // NOTE: Exporting from Krita requires to flip the image to come right-side up here
  inline bool Read(const std::string& filename, Header& header, std::vector<uint8_t>& pixels)
  {
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
      Logger::LogErr("[TGA] Could not open file: " + filename);
      return false;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(Header));

    // std::cout << header.width << std::endl;

    // Ensure the image type is uncompressed RGB (2 or 10)
    if (header.imageType != 2 && header.imageType != 10)
    {
      Logger::LogErr("[TGA] Unsupported TGA image type: " + std::to_string(static_cast<int>(header.imageType)));
      return false;
    }

    // Move the file pointer to the pixel data
    file.seekg(header.idLength + sizeof(Header));

    // Read pixel data
    pixels = std::vector<std::uint8_t>(header.width * header.height * (header.bitsPerPixel / 8));
    file.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
    // for (size_t i = 0; i < pixels.size(); i += 4)
    // {
    //   auto b = pixels[i];
    //   auto g = pixels[i + 1];
    //   auto r = pixels[i + 2];
    //   auto a = pixels[i + 3];
    //   pixels[i] = r;
    //   pixels[i + 1] = g;
    //   pixels[i + 2] = b;
    //   pixels[i + 3] = a;
    // }

    return true;
  }
}