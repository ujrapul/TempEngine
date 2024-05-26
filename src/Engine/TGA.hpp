// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "String.hpp"
#include "Logger.hpp"
#include "Array.hpp" // IWYU pragma: keep
#include <cstdio>

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
  inline bool Read(const String& filename, Header& header, DynamicArray<uint8_t>& pixels)
  {
    FILE* file = fopen(filename.c_str(), "rb");
    
    if (!file)
    {
      Logger::LogErr(String("[TGA] Could not open file: ") + filename);
      return false;
    }

    if (fread(&header, sizeof(Header), 1, file) != 1)
    {
      Logger::LogErr(String("[TGA] Failed to read header for file: ") + filename);
      goto end;
    }

    // std::cout << header.width << std::endl;

    // Ensure the image type is uncompressed RGB (2 or 10)
    if (header.imageType != 2 && header.imageType != 10)
    {
      Logger::LogErr(String("[TGA] Unsupported TGA image type: ") +
                     String::ToString(static_cast<int>(header.imageType)));
      goto end;
    }

    // Move the file pointer to the pixel data
    if (fseek(file, header.idLength + sizeof(struct Header), SEEK_SET) != 0)
    {
      Logger::LogErr(String("[TGA] Failed to seek to pixel data."));
      goto end;
    }

    // Read pixel data
    pixels.Resize((size_t)(header.width * header.height * (header.bitsPerPixel / 8)));
    if (fread(pixels.buffer, 1, pixels.size, file) != pixels.size)
    {
      Logger::LogErr(String("[TGA] Failed to read pixel data."));
      goto end;
    }
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

end:
    fclose(file);
    return false;
  }
}