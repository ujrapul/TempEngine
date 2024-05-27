// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdio>

namespace Temp
{
  struct Stream
  {
    // Meant for booleans
    constexpr Stream& operator<<(unsigned char data)
    {
      printf(data ? "True" : "False");
      return *this;
    }

    constexpr Stream& operator<<(int data)
    {
      printf("%d", data);
      return *this;
    }

    constexpr Stream& operator<<(float data)
    {
      printf("%f", data);
      return *this;
    }

    constexpr Stream& operator<<(size_t data)
    {
      printf("%zu", data);
      return *this;
    }

    constexpr Stream& operator<<(unsigned int data)
    {
      printf("%u", data);
      return *this;
    }

    constexpr Stream& operator<<(const char* const data)
    {
      printf("%s", data);
      return *this;
    }

    constexpr Stream& operator<<(void* data)
    {
      printf("%p", data);
      return *this;
    }
  };

  inline Stream inout;
}