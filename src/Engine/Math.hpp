// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4201)
#pragma warning(disable : 4234)
#pragma warning(disable : 4324)
#pragma warning(disable : 4459)
#pragma warning(disable : 26495)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "Math_impl.hpp"
#include "STDPCH.hpp"

namespace Temp::Math
{
  extern template struct Vec2<int>;
  extern template struct Vec2<unsigned int>;
  extern template struct Vec2<float>;

  typedef Vec2<float> Vec2f;
  typedef Vec2<unsigned int> Vec2ui;
  typedef Vec2<int> Vec2i;

  extern template struct Vec3<float>;
  extern template struct Vec3<unsigned char>;

  typedef Vec3<float> Vec3f;
  typedef Vec3<unsigned char> Vec3i;

  extern template struct Vec4<float>;
  extern template struct Vec4<unsigned char>;

  typedef Vec4<float> Vec4f;
  typedef Vec4<unsigned char> Vec4i;
}

extern template struct Temp::DynamicArray<Temp::Math::Vec3<float>, Temp::MemoryManager::Data::SCENE_ARENA>;
extern template struct Temp::DynamicArray<Temp::DynamicArray<Temp::Math::Vec3<float>, Temp::MemoryManager::Data::SCENE_ARENA>>;

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
