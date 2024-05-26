// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp" // IWYU pragma: keep

namespace Temp
{
  using ComponentBits = uint64_t;

  constexpr void SetBit(ComponentBits& componentBits, int bit)
  {
    componentBits |= (1ULL << bit);
  }

  constexpr void ClearBit(ComponentBits& componentBits, int bit)
  {
    componentBits &= ~(1ULL << bit);
  }

  constexpr bool Test(const ComponentBits& componentBits, int bit)
  {
    return (componentBits & (1ULL << bit)) != 0;
  }
}
