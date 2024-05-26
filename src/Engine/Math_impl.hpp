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

#include "Math_fwd.hpp"

namespace Temp::Math
{
  template <typename T>
  Vec2<T>::Vec2() : x(0), y(0)
  {
  }

  template <typename T>
  Vec2<T>::Vec2(T x, T y) : x(x), y(y)
  {
  }

  template <typename T>
  Vec2<T>::Vec2(__m128 data) : simdData(data)
  {
  }

  template <typename T>
  Vec2<T> Vec2<T>::operator+(const Vec2& other) const
  {
    return _mm_add_ps(simdData, other.simdData);
  }

  // Vector-vector subtraction
  template <typename T>
  Vec2<T> Vec2<T>::operator-(const Vec2& other) const
  {
    return _mm_sub_ps(simdData, other.simdData);
  }

  // Vector-vector multiplication
  template <typename T>
  Vec2<T> Vec2<T>::operator*(const Vec2& other) const
  {
    return _mm_mul_ps(simdData, other.simdData);
  }

  // Vector-vector division
  template <typename T>
  Vec2<T> Vec2<T>::operator/(const Vec2& other) const
  {
    return _mm_div_ps(simdData, other.simdData);
  }

  // Vector-float addition
  template <typename T>
  Vec2<T> Vec2<T>::operator+(T scalar) const
  {
    return {(T)(x + scalar), (T)(y + scalar)};
  }

  // Vector-float subtraction
  template <typename T>
  Vec2<T> Vec2<T>::operator-(T scalar) const
  {
    return {(T)(x - scalar), (T)(y - scalar)};
  }

  // Vector-float multiplication
  template <typename T>
  Vec2<T> Vec2<T>::operator*(T scalar) const
  {
    return {(T)(x * scalar), (T)(y * scalar)};
  }

  // Vector-float division
  template <typename T>
  Vec2<T> Vec2<T>::operator/(T scalar) const
  {
    return {(T)(x / scalar), (T)(y / scalar)};
  }

  template <typename T>
  bool Vec2<T>::operator==(const Vec2& other) const
  {
    return FloatEqual(x, other.x) && FloatEqual(y, other.y);
  }

  // Dot product of two vectors
  template <typename T>
  T Vec2<T>::dot(const Vec2<T>& other) const
  {
    return (T)(x * other.x + y * other.y);
  }

  // Magnitude (length) of the vector
  template <typename T>
  T Vec2<T>::magnitude() const
  {
    return Sqrt(dot(*this));
  }

  // Squared magnitude (length) of the vector
  template <typename T>
  T Vec2<T>::magnitude2() const
  {
    return dot(*this);
  }

  // Normalize the vector
  template <typename T>
  Vec2<T> Vec2<T>::normalize() const
  {
    float mag = magnitude();
    float invMagnitude = 1.0f / (mag + (mag == 0.0f));
    return {(T)(x * invMagnitude), (T)(y * invMagnitude)};
  }

  template <typename T>
  Vec3<T>::Vec3() : x(0), y(0), z(0)
  {
  }

  template <typename T>
  Vec3<T>::Vec3(T value) : x(value), y(value), z(value)
  {
  }

  template <typename T>
  Vec3<T>::Vec3(T x, T y) : x(x), y(y), z(0)
  {
  }

  template <typename T>
  Vec3<T>::Vec3(T x, T y, T z) : x(x), y(y), z(z)
  {
  }

  template <typename T>
  Vec3<T>::Vec3(__m128 data) : simdData(data)
  {
  }

  // Vector-vector addition
  template <typename T>
  Vec3<T> Vec3<T>::operator+(const Vec3& other) const
  {
    return _mm_add_ps(simdData, other.simdData);
  }

  // Vector-vector subtraction
  template <typename T>
  Vec3<T> Vec3<T>::operator-(const Vec3& other) const
  {
    return _mm_sub_ps(simdData, other.simdData);
  }

  // Vector-vector multiplication
  template <typename T>
  Vec3<T> Vec3<T>::operator*(const Vec3& other) const
  {
    return _mm_mul_ps(simdData, other.simdData);
  }

  // Vector-vector division
  template <typename T>
  Vec3<T> Vec3<T>::operator/(const Vec3& other) const
  {
    return _mm_div_ps(simdData, other.simdData);
  }

  // Vector-float addition
  template <typename T>
  Vec3<T> Vec3<T>::operator+(T scalar) const
  {
    return {(T)(x + scalar), (T)(y + scalar), (T)(z + scalar)};
  }

  // Vector-float subtraction
  template <typename T>
  Vec3<T> Vec3<T>::operator-(T scalar) const
  {
    return {(T)(x - scalar), (T)(y - scalar), (T)(z - scalar)};
  }

  // Vector-float multiplication
  template <typename T>
  Vec3<T> Vec3<T>::operator*(T scalar) const
  {
    return {(T)(x * scalar), (T)(y * scalar), (T)(z * scalar)};
  }

  // Vector-float division
  template <typename T>
  Vec3<T> Vec3<T>::operator/(T scalar) const
  {
    return {(T)(x / scalar), (T)(y / scalar), (T)(z / scalar)};
  }

  template <typename T>
  T& Vec3<T>::operator[](int i)
  {
    return data[i];
  }

  // Dot product of two vectors
  template <typename T>
  T Vec3<T>::dot(const Vec3& other) const
  {
    return (T)(x * other.x + y * other.y + z * other.z);
  }

  template <typename T>
  bool Vec3<T>::operator==(const Vec3& other) const
  {
    return FloatEqual(x, other.x) && FloatEqual(y, other.y) && FloatEqual(z, other.z);
  }

  template <typename T>
  // Cross product of two vectors
  Vec3<T> Vec3<T>::cross(const Vec3& other) const
  {
    return {(T)((y * other.z) - (z * other.y)),
            (T)((z * other.x) - (x * other.z)),
            (T)((x * other.y) - (y * other.x))};
  }

  template <typename T>
  // Magnitude (length) of the vector
  T Vec3<T>::magnitude() const
  {
    return Sqrt(dot(*this));
  }

  template <typename T>
  // Squared magnitude (length) of the vector
  T Vec3<T>::magnitude2() const
  {
    return dot(*this);
  }

  template <typename T>
  // Normalize the vector
  Vec3<T> Vec3<T>::normalize() const
  {
    float mag = magnitude();
    float invMagnitude = 1.0f / (mag + (mag == 0.0f));
    return {(T)(x * invMagnitude), (T)(y * invMagnitude), (T)(z * invMagnitude)};
  }

  extern template struct Vec3<float>;
  extern template struct Vec3<unsigned char>;

  typedef Vec3<float> Vec3f;
  typedef Vec3<unsigned char> Vec3i;

  // 4D vector struct
  template <typename T>
  Vec4<T>::Vec4() : x(0), y(0), z(0), w(0)
  {
  }

  template <typename T>
  Vec4<T>::Vec4(T value) : x(value), y(value), z(value), w(value)
  {
  }

  template <typename T>
  Vec4<T>::Vec4(T x, T y) : x(x), y(y), z(0), w(0)
  {
  }

  template <typename T>
  Vec4<T>::Vec4(T x, T y, T z) : x(x), y(y), z(z), w(0)
  {
  }

  template <typename T>
  Vec4<T>::Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w)
  {
  }

  template <typename T>
  Vec4<T>::Vec4(const Vec3<T> vec3) : x(vec3.x), y(vec3.y), z(vec3.z), w(0)
  {
  }

  template <typename T>
  Vec4<T>::Vec4(__m128 data) : simdData(data)
  {
  }

  // Vector-vector addition
  template <typename T>
  Vec4<T> Vec4<T>::operator+(const Vec4& other) const
  {
    return _mm_add_ps(simdData, other.simdData);
  }

  // Vector-vector subtraction
  template <typename T>
  Vec4<T> Vec4<T>::operator-(const Vec4& other) const
  {
    return _mm_sub_ps(simdData, other.simdData);
  }

  // Vector-vector multiplication
  template <typename T>
  Vec4<T> Vec4<T>::operator*(const Vec4& other) const
  {
    return _mm_mul_ps(simdData, other.simdData);
  }

  // Vector-vector division
  template <typename T>
  Vec4<T> Vec4<T>::operator/(const Vec4& other) const
  {
    return _mm_div_ps(simdData, other.simdData);
  }

  // Vector-matrix multiplication
  template <typename T>
  Vec4<T> Vec4<T>::operator*(const Mat4& mat) const
  {
    float resultX = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + w * mat[3][0];
    float resultY = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + w * mat[3][1];
    float resultZ = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + w * mat[3][2];
    float resultW = x * mat[0][3] + y * mat[1][3] + z * mat[2][3] + w * mat[3][3];

    return Vec4<T>(resultX, resultY, resultZ, resultW);
  }

  // Vector-float addition
  template <typename T>
  Vec4<T> Vec4<T>::operator+(T scalar) const
  {
    __m128 scalarData = _mm_set1_ps(scalar);
    return _mm_add_ps(simdData, scalarData);
  }

  // Vector-float subtraction
  template <typename T>
  Vec4<T> Vec4<T>::operator-(T scalar) const
  {
    __m128 scalarData = _mm_set1_ps(scalar);
    return _mm_sub_ps(simdData, scalarData);
  }

  // Vector-float multiplication
  template <typename T>
  Vec4<T> Vec4<T>::operator*(T scalar) const
  {
    __m128 scalarData = _mm_set1_ps(scalar);
    return _mm_mul_ps(simdData, scalarData);
  }

  // Vector-float division
  template <typename T>
  Vec4<T> Vec4<T>::operator/(T scalar) const
  {
    __m128 scalarData = _mm_set1_ps(scalar);
    return _mm_div_ps(simdData, scalarData);
  }

  template <typename T>
  const T& Vec4<T>::operator[](int i) const
  {
    return data[i];
  }

  template <typename T>
  T& Vec4<T>::operator[](int i)
  {
    return data[i];
  }

  template <typename T>
  bool Vec4<T>::operator==(const Vec4& other) const
  {
    return FloatEqual(x, other.x) && FloatEqual(y, other.y) && FloatEqual(z, other.z) &&
           FloatEqual(w, other.w);
  }

  // Dot product of two vectors
  template <typename T>
  T Vec4<T>::dot(const Vec4& other) const
  {
    return _mm_cvtss_f32(_mm_dp_ps(simdData, other.simdData, 0xFF));
  }

  // Magnitude (length) of the vector
  template <typename T>
  T Vec4<T>::magnitude() const
  {
    __m128 squaredLength = _mm_dp_ps(simdData, simdData, 0xFF);
    return _mm_cvtss_f32(_mm_sqrt_ss(squaredLength));
  }

  // Squared magnitude (length) of the vector
  template <typename T>
  T Vec4<T>::magnitude2() const
  {
    return dot(*this);
  }

  // Normalize the vector
  template <typename T>
  Vec4<T> Vec4<T>::normalize() const
  {
    return _mm_mul_ps(simdData, _mm_rsqrt_ps(_mm_dp_ps(simdData, simdData, 0xFF)));
  }

  extern template struct Vec4<float>;
  extern template struct Vec4<unsigned char>;

  typedef Vec4<float> Vec4f;
  typedef Vec4<unsigned char> Vec4i;
}

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
