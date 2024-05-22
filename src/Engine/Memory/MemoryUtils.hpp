// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

namespace Temp::Utils
{
  template<typename T>
  constexpr void Swap(T& first, T& second)
  {
    T other = first;
    first = second;
    second = other;
  }
}