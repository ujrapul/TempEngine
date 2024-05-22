// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array.hpp"
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

#include "STDPCH.hpp"

namespace Temp::Math
{
  constexpr float PI = 3.141592653589793238f;
  constexpr float EPSILON = 0.0001f;

  template<typename T>
  constexpr T Abs(T x)
  {
    if (x < 0)
    {
      return -x;
    }
    return x;
  }

  constexpr double Sqrt(double x)
  {
    if (x == 0 || x == 1)
    {
      return x;
    }

    double guess = x / 2;
    double prevGuess = 0;

    do
    {
      prevGuess = guess;
      guess = (guess + x / guess) / 2;
    } while (Abs(guess - prevGuess) >= 0.00001); // Adjust the precision as needed

    return guess;
  }

  constexpr int Ceil(double x)
  {
    int result = static_cast<int>(x);
    return (result < x) ? (result + 1) : result;
  }

  constexpr int Floor(double x)
  {
    int result = static_cast<int>(x);
    return (result > x) ? (result - 1) : result;
  }

  template <typename T>
  constexpr T Max(T a, T b)
  {
    return a > b ? a : b;
  }

  template <typename T>
  constexpr T Min(T a, T b)
  {
    return a < b ? a : b;
  }

  constexpr bool FloatEqual(float a, float b)
  {
    return Abs(a - b) < EPSILON;
  }

  template <typename T>
  struct Vec2
  {
    union
    {
      struct
      {
        T x;
        T y;
      };
      T data[2];
      __m128 simdData;
    };

    Vec2();
    Vec2(T x, T y);
    Vec2(__m128 data);

    Vec2 operator+(const Vec2 &other) const;

    // Vector-vector subtraction
    Vec2 operator-(const Vec2 &other) const;

    // Vector-vector multiplication
    Vec2 operator*(const Vec2 &other) const;

    // Vector-vector division
    Vec2 operator/(const Vec2 &other) const;

    // Vector-float addition
    Vec2 operator+(T scalar) const;

    // Vector-float subtraction
    Vec2 operator-(T scalar) const;

    // Vector-float multiplication
    Vec2 operator*(T scalar) const;

    // Vector-float division
    Vec2 operator/(T scalar) const;

    bool operator==(const Vec2 &other) const;

    // Dot product of two vectors
    T dot(const Vec2 &other) const;

    // Magnitude (length) of the vector
    T magnitude() const;

    // Squared magnitude (length) of the vector
    T magnitude2() const;

    // Normalize the vector
    Vec2 normalize() const;
  };

  template <typename T>
  inline std::ostream &operator<<(std::ostream &os, const Vec2<T> &vec2)
  {
    os << "Vec2(" << vec2.x << " " << vec2.y << ")";
    return os;
  }

  template <typename T>
  struct Vec3
  {
    union
    {
      struct
      {
        T x;
        T y;
        T z;
      };
      T data[3];
      __m128 simdData;
    };

    // 3D vector struct
    Vec3();
    Vec3(T value);
    Vec3(T x, T y);
    Vec3(T x, T y, T z);
    Vec3(__m128 data);

    // Vector-vector addition
    Vec3 operator+(const Vec3 &other) const;

    // Vector-vector subtraction
    Vec3 operator-(const Vec3 &other) const;

    // Vector-vector multiplication
    Vec3 operator*(const Vec3 &other) const;

    // Vector-vector division
    Vec3 operator/(const Vec3 &other) const;

    // Vector-float addition
    Vec3 operator+(T scalar) const;

    // Vector-float subtraction
    Vec3 operator-(T scalar) const;

    // Vector-float multiplication
    Vec3 operator*(T scalar) const;

    // Vector-float division
    Vec3 operator/(T scalar) const;

    T &operator[](int i);

    // Dot product of two vectors
    T dot(const Vec3 &other) const;

    bool operator==(const Vec3 &other) const;

    // Cross product of two vectors
    Vec3 cross(const Vec3 &other) const;

    // Magnitude (length) of the vector
    T magnitude() const;

    // Squared magnitude (length) of the vector
    T magnitude2() const;

    // Normalize the vector
    Vec3 normalize() const;
  };

  template <typename T>
  inline std::ostream &operator<<(std::ostream &os, const Vec3<T> &vec3)
  {
    os << "Vec3(" << vec3.x << " " << vec3.y << " " << vec3.z << ")";
    return os;
  }

  struct Mat4;

  template <typename T>
  struct Vec4
  {
    union
    {
      struct
      {
        T x;
        T y;
        T z;
        T w;
      };
      T data[4];
      __m128 simdData;
    };

    // 4D vector struct
    Vec4();
    Vec4(T value);
    Vec4(T x, T y);
    Vec4(T x, T y, T z);
    Vec4(T x, T y, T z, T w);
    Vec4(const Vec3<T> vec3);
    Vec4(__m128 data);

    // Vector-vector addition
    Vec4 operator+(const Vec4 &other) const;
    
    // Vector-vector subtraction
    Vec4 operator-(const Vec4 &other) const;

    // Vector-vector multiplication
    Vec4 operator*(const Vec4 &other) const;

    // Vector-vector division
    Vec4 operator/(const Vec4 &other) const;

    Vec4 operator*(const Mat4 &mat) const;

    // Vector-float addition
    Vec4 operator+(T scalar) const;

    // Vector-float subtraction
    Vec4 operator-(T scalar) const;

    // Vector-float multiplication
    Vec4 operator*(T scalar) const;

    // Vector-float division
    Vec4 operator/(T scalar) const;

    const T &operator[](int i) const;

    T &operator[](int i);

    bool operator==(const Vec4 &other) const;

    // Dot product of two vectors
    T dot(const Vec4 &other) const;

    // Magnitude (length) of the vector
    T magnitude() const;

    // Squared magnitude (length) of the vector
    T magnitude2() const;

    // Normalize the vector
    Vec4 normalize() const;
  };

  template <typename T>
  inline std::ostream &operator<<(std::ostream &os, const Vec4<T> &vec4)
  {
    os << "Vec4(" << vec4.x << " " << vec4.y << " " << vec4.z << " " << vec4.w << ")";
    return os;
  }

  struct Mat2
  {
    Vec2<float> rows[2];

    // 2x2 matrix struct
    Mat2();
    Mat2(const Vec2<float> &col1, const Vec2<float> &col2);

    // Matrix-matrix addition
    Mat2 operator+(const Mat2 &other) const;

    // Matrix-matrix subtraction
    Mat2 operator-(const Mat2 &other) const;

    // Matrix-matrix multiplication
    Mat2 operator*(const Mat2 &mat) const;

    // Matrix-float multiplication
    Mat2 operator*(float scalar) const;

    // Matrix-vector multiplication
    Vec2<float> operator*(const Vec2<float> &vec) const;

    bool operator==(const Mat2 &other) const;

    // Matrix transposition
    Mat2 transpose() const;

    // Matrix determinant
    float determinant() const;

    // Matrix inverse
    Mat2 inverse() const;
  };

  // 3x3 matrix struct
  struct Mat3
  {
    Vec3<float> rows[3];

    // 3x3 matrix struct
    Mat3();
    Mat3(const Vec3<float> &col1, const Vec3<float> &col2, const Vec3<float> &col3);

    // Matrix-matrix addition
    Mat3 operator+(const Mat3 &other) const;

    // Matrix-matrix subtraction
    Mat3 operator-(const Mat3 &other) const;

    // Matrix-matrix multiplication using SIMD
    Mat3 operator*(const Mat3 &mat) const;

    // Matrix-float multiplication
    Mat3 operator*(float scalar) const;

    // Matrix-vector multiplication
    Vec3<float> operator*(const Vec3<float> &vec) const;

    bool operator==(const Mat3 &other) const;

    // Matrix transposition
    Mat3 transpose() const;

    // Matrix determinant
    float determinant() const;

    // Matrix inverse
    Mat3 inverse() const;

    void print(const Mat3 &mat3);
  };

  struct Mat4
  {
    Vec4<float> rows[4];

    // 4x4 matrix struct
    Mat4();
    Mat4(const Vec4<float> &col1, const Vec4<float> &col2, const Vec4<float> &col3, const Vec4<float> &col4);

    // Matrix-matrix addition
    Mat4 operator+(const Mat4 &other) const;

    // Matrix-matrix subtraction
    Mat4 operator-(const Mat4 &other) const;

    // Matrix-matrix multiplication
    Mat4 operator*(const Mat4 &mat) const;

    // Matrix-float multiplication
    Mat4 operator*(float scalar) const;

    // Matrix-vector multiplication
    Vec4<float> operator*(const Vec4<float> &vec) const;

    const Vec4<float> &operator[](int i) const;

    Vec4<float> &operator[](int i);

    bool operator==(const Mat4 &other) const;

    // Matrix transposition
    Mat4 transpose() const;

    // Matrix determinant
    float determinant() const;

    // Matrix inverse
    Mat4 inverse() const;

    Mat4 translate(const Vec3<float> &translation);

    void setTranslation(const Vec3<float> &translation);

    Mat4 scale(const Vec3<float> &scale);

    void setScale(const Vec3<float> &scale);

    // Rotations take radians!

    Mat4 rotateX(float angle);

    Mat4 rotateY(float angle);

    Mat4 rotateZ(float angle);

    Mat4 rotate(float angle, const Vec3<float> &axis);

    // Need to underscore to avoid conflict with near and far defines in windows
    static Mat4 perspective(float fov, float aspectRatio, float _near, float _far);

    static Mat4 orthographic(float left, float right, float bottom, float top, float _near, float _far);
  };

  inline std::ostream &operator<<(std::ostream &os, const Mat4 &mat4)
  {
    os << "Mat4\n"
       << mat4.rows[0].x << "\t" << mat4.rows[0].y << "\t" << mat4.rows[0].z << "\t" << mat4.rows[0].w << "\n"
       << mat4.rows[1].x << "\t" << mat4.rows[1].y << "\t" << mat4.rows[1].z << "\t" << mat4.rows[1].w << "\n"
       << mat4.rows[2].x << "\t" << mat4.rows[2].y << "\t" << mat4.rows[2].z << "\t" << mat4.rows[2].w << "\n"
       << mat4.rows[3].x << "\t" << mat4.rows[3].y << "\t" << mat4.rows[3].z << "\t" << mat4.rows[3].w;
    return os;
  }

  constexpr float ToRadians(float degrees)
  {
    return degrees * PI / 180.f;
  }

  bool RayTriangleIntersect(const Vec3<float>& rayOrigin,
                            const Vec3<float>& rayDirection,
                            const Vec3<float>& v0,
                            const Vec3<float>& v1,
                            const Vec3<float>& v2,
                            Vec3<float>& intersectionPoint);

  bool RayMeshIntersect(
    const Vec3<float>& rayOrigin,
    const Vec3<float>& rayDirection,
    const SceneDynamicArray<SceneDynamicArray<Vec3<float>>>& triangles,
    Vec3<float>& intersectionPoint);
}

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
