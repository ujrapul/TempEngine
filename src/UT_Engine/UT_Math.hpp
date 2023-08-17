// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4101) // C4101: 'variable': unreferenced local variable
#pragma warning(disable: 4189) // C4189: 'variable': local variable is initialized but not referenced
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include "Math.hpp"

namespace Temp::Math
{
  namespace UnitTests
  {
    // Function to compare floating-point values with a small epsilon
    bool floatEquals(float a, float b)
    {
      return FloatEqual(a, b);
    }

    // Function to compare Vec2 objects
    bool vec2Equals(const Vec2f &v1, const Vec2f &v2)
    {
      return v1 == v2;
    }

    // Function to compare Vec3 objects
    bool vec3Equals(const Vec3f &v1, const Vec3f &v2)
    {
      return v1 == v2;
    }

    // Function to compare Vec4 objects
    bool vec4Equals(const Vec4f &v1, const Vec4f &v2)
    {
      return v1 == v2;
    }

    // Function to compare Mat2 objects
    bool mat2Equals(const Mat2 &m1, const Mat2 &m2)
    {
      return m1 == m2;
    }

    // Function to compare Mat3 objects
    bool mat3Equals(const Mat3 &m1, const Mat3 &m2)
    {
      return m1 == m2;
    }

    // Function to compare Mat4 objects
    bool mat4Equals(const Mat4 &m1, const Mat4 &m2)
    {
      return m1 == m2;
    }

    // Function to run the unit tests
    void Run()
    {
      assert(Sqrt(4) == 2);
      assert(Sqrt(100) == 10);
      assert(Sqrt(763) == 27.6224546339 || Abs(Sqrt(763) - 27.6225) < 0.0001);

      // Test Vec2 operations
      Vec2f v2a(2.0f, 3.0f);
      Vec2f v2b(4.0f, 5.0f);

      // Addition
      Vec2 v2add = v2a + v2b;
      assert(vec2Equals(v2add, Vec2(6.0f, 8.0f)));

      // Subtraction
      Vec2 v2sub = v2a - v2b;
      assert(vec2Equals(v2sub, Vec2(-2.0f, -2.0f)));

      // Multiplication
      Vec2 v2mul = v2a * v2b;
      assert(vec2Equals(v2mul, Vec2(8.0f, 15.0f)));

      // Division
      Vec2 v2div = v2a / v2b;
      assert(vec2Equals(v2div, Vec2(0.5f, 0.6f)));

      // Dot Product
      float dotProduct2 = v2a.dot(v2b);
      assert(Abs(dotProduct2 - 23.0f) < 0.0001f);

      // Magnitude
      float magnitude2 = v2a.magnitude();
      assert(Abs(magnitude2 - 3.6056f) < 0.0001f);

      // Magnitude2
      float magnitude22 = v2a.magnitude2();
      assert(Abs(magnitude22 - 13) < 0.0001f);

      // Normalization
      assert(vec2Equals(v2a.normalize(), {0.5547f, 0.832f}));

      // Test Vec3 operations
      Vec3f v3a(2.0f, 3.0f, 4.0f);
      Vec3f v3b(4.0f, 5.0f, 6.0f);

      // Operator[]
      assert(v3a[0] == 2.0f && v3a[1] == 3.0f && v3a[2] == 4.0f);

      // Addition
      Vec3 v3add = v3a + v3b;
      assert(vec3Equals(v3add, Vec3(6.0f, 8.0f, 10.0f)));

      // Subtraction
      Vec3 v3sub = v3a - v3b;
      assert(vec3Equals(v3sub, Vec3(-2.0f, -2.0f, -2.0f)));

      // Multiplication
      Vec3 v3mul = v3a * v3b;
      assert(vec3Equals(v3mul, Vec3(8.0f, 15.0f, 24.0f)));

      // Division
      Vec3 v3div = v3a / v3b;
      assert(vec3Equals(v3div, Vec3(0.5f, 0.6f, 0.6667f)));

      // Dot Product
      float dotProduct3 = v3a.dot(v3b);
      assert(std::abs(dotProduct3 - 47.0f) < 0.0001f);

      // Cross Product
      Vec3 crossProduct3 = v3a.cross(v3b);
      Vec3 expectedCrossProduct(-2.0f, 4.0f, -2.0f);
      assert(vec3Equals(crossProduct3, expectedCrossProduct));

      // Magnitude
      float magnitude3 = v3a.magnitude();
      assert(std::abs(magnitude3 - 5.38516f) < 0.0001f);

      // Magnitude2
      float magnitude32 = v3a.magnitude2();
      assert(std::abs(magnitude32 - (5.38516f * 5.38516f)) < 0.0001f);

      // Normalization
      assert(vec3Equals(v3a.normalize(), {0.3714f, 0.5571f, 0.7428f}));

      // Test Vec4 operations
      Vec4 v4a(2.0f, 3.0f, 4.0f, 5.0f);
      Vec4 v4b(4.0f, 5.0f, 6.0f, 7.0f);

      // Operator[]
      assert(v4a[0] == 2.0f && v4a[1] == 3.0f && v4a[2] == 4.0f && v4a[3] == 5.0f);

      // Addition
      Vec4 v4add = v4a + v4b;
      assert(vec4Equals(v4add, Vec4(6.0f, 8.0f, 10.0f, 12.0f)));

      // Subtraction
      Vec4 v4sub = v4a - v4b;
      assert(vec4Equals(v4sub, Vec4(-2.0f, -2.0f, -2.0f, -2.0f)));

      // Multiplication
      Vec4 v4mul = v4a * v4b;
      assert(vec4Equals(v4mul, Vec4(8.0f, 15.0f, 24.0f, 35.0f)));

      // Division
      Vec4 v4div = v4a / v4b;
      assert(vec4Equals(v4div, Vec4(0.5f, 0.6f, 0.6667f, 0.7143f)));

      // Dot Product
      float dotProduct4 = v4a.dot(v4b);
      assert(std::abs(dotProduct4 - 82.f) < 0.0001f);

      // Magnitude
      float magnitude4 = v4a.magnitude();
      assert(std::abs(magnitude4 - 7.34847f) < 0.0001f);

      // Magnitude2
      float magnitude42 = v4a.magnitude2();
      assert(std::abs(magnitude42 - (7.34847f * 7.34847f)) < 0.0001f);

      // Normalization
      assert(vec4Equals(v4a.normalize(), {0.272165f, 0.408248f, 0.544331f, 0.680414f}));

      // Test Mat2 operations
      Mat2 m2a(Vec2(1.0f, 2.0f), Vec2(3.0f, 4.0f));
      Mat2 m2b(Vec2(5.0f, 6.0f), Vec2(7.0f, 8.0f));

      // Addition
      Mat2 m2add = m2a + m2b;
      assert(mat2Equals(m2add, Mat2(Vec2(6.0f, 8.0f), Vec2(10.0f, 12.0f))));

      // Subtraction
      Mat2 m2sub = m2a - m2b;
      assert(mat2Equals(m2sub, Mat2(Vec2(-4.0f, -4.0f), Vec2(-4.0f, -4.0f))));

      // Multiplication
      Mat2 m2mul = m2a * m2b;
      assert(mat2Equals(m2mul, Mat2(Vec2(19.0f, 22.0f), Vec2(43.0f, 50.0f))));

      // Scalar multiplication
      Mat2 m2scalar = m2a * 2.0f;
      assert(mat2Equals(m2scalar, Mat2(Vec2(2.0f, 4.0f), Vec2(6.0f, 8.0f))));

      // Matrix-vector multiplication
      Vec2 v2(2.0f, 3.0f);
      Vec2 m2vec = m2a * v2;
      assert(vec2Equals(m2vec, Vec2(8.0f, 18.0f)));

      // Transpose
      Mat2 m2transpose = m2a.transpose();
      assert(mat2Equals(m2transpose, Mat2(Vec2(1.0f, 3.0f), Vec2(2.0f, 4.0f))));

      // Determinant
      float m2det = m2a.determinant();
      assert(floatEquals(m2det, -2.0f));

      // Inverse
      Mat2 m2inverse = m2a.inverse();
      assert(mat2Equals(m2inverse, Mat2(Vec2(-2.0f, 1.0f), Vec2(1.5f, -0.5f))));

      // Test Mat3 operations
      Mat3 m3a(Vec3(1.0f, 2.0f, 3.0f), Vec3(4.0f, 5.0f, 6.0f), Vec3(7.0f, 8.0f, 9.0f));
      Mat3 m3b(Vec3(10.0f, 11.0f, 12.0f), Vec3(13.0f, 14.0f, 15.0f), Vec3(16.0f, 17.0f, 18.0f));
      Mat3 m3c(Vec3(1.0f, 2.0f, 0.0f), Vec3(0.0f, 1.0f, 2.0f), Vec3(2.0f, 0.0f, 1.0f));

      // Addition
      Mat3 m3add = m3a + m3b;
      assert(mat3Equals(m3add, Mat3(Vec3(11.0f, 13.0f, 15.0f), Vec3(17.0f, 19.0f, 21.0f), Vec3(23.0f, 25.0f, 27.0f))));

      // Subtraction
      Mat3 m3sub = m3a - m3b;
      assert(mat3Equals(m3sub, Mat3(Vec3(-9.0f, -9.0f, -9.0f), Vec3(-9.0f, -9.0f, -9.0f), Vec3(-9.0f, -9.0f, -9.0f))));

      // Multiplication
      Mat3 m3mul = m3a * m3b;
      assert(mat3Equals(m3mul, Mat3(Vec3(84.0f, 90.0f, 96.0f), Vec3(201.0f, 216.0f, 231.0f), Vec3(318.0f, 342.0f, 366.0f))));

      // Scalar multiplication
      Mat3 m3scalar = m3a * 2.0f;
      assert(mat3Equals(m3scalar, Mat3(Vec3(2.0f, 4.0f, 6.0f), Vec3(8.0f, 10.0f, 12.0f), Vec3(14.0f, 16.0f, 18.0f))));

      // Matrix-vector multiplication
      Vec3 v3(1.0f, 2.0f, 3.0f);
      Vec3 m3vecmul = m3a * v3;
      assert(vec3Equals(m3vecmul, Vec3(14.0f, 32.0f, 50.0f)));

      // Transpose
      Mat3 m3transpose = m3a.transpose();
      assert(mat3Equals(m3transpose, Mat3(Vec3(1.0f, 4.0f, 7.0f), Vec3(2.0f, 5.0f, 8.0f), Vec3(3.0f, 6.0f, 9.0f))));

      // Determinant
      float det = m3c.determinant();
      assert(floatEquals(det, 9.0f)); // Example determinant value, adjust accordingly

      // Inverse
      Mat3 m3inverse = m3c.inverse();
      assert(mat3Equals(m3inverse, Mat3(Vec3(0.11111111111111111112f, -0.2222222222222222223f, 0.44444444444444444444f),
                                        Vec3(0.44444444444444444444f, 0.11111111111111111112f, -0.22222222222222222222f),
                                        Vec3(-0.22222222222222222222f, 0.44444444444444444444f, 0.11111111111111111111f))));

      // Test Mat4 operations
      Mat4 m4a(Vec4(1.0f, 2.0f, 3.0f, 4.0f),
               Vec4(5.0f, 6.0f, 7.0f, 8.0f),
               Vec4(9.0f, 10.0f, 11.0f, 12.0f),
               Vec4(13.0f, 14.0f, 15.0f, 16.0f));
      Mat4 m4b(Vec4(17.0f, 18.0f, 19.0f, 20.0f),
               Vec4(21.0f, 22.0f, 23.0f, 24.0f),
               Vec4(25.0f, 26.0f, 27.0f, 28.0f),
               Vec4(29.0f, 30.0f, 31.0f, 32.0f));
      Mat4 m4c(Vec4(0.0f, 2.0f, 3.0f, 4.0f),
               Vec4(5.0f, -6.0f, 7.0f, 8.0f),
               Vec4(9.0f, 10.0f, 11.0f, 12.0f),
               Vec4(13.0f, 7.0f, 15.0f, 17.0f));

      // Addition
      Mat4 m4add = m4a + m4b;
      assert(mat4Equals(m4add, Mat4(Vec4(18.0f, 20.0f, 22.0f, 24.0f), Vec4(26.0f, 28.0f, 30.0f, 32.0f), Vec4(34.0f, 36.0f, 38.0f, 40.0f), Vec4(42.0f, 44.0f, 46.0f, 48.0f))));

      // Subtraction
      Mat4 m4sub = m4a - m4b;
      assert(mat4Equals(m4sub, Mat4(Vec4(-16.0f, -16.0f, -16.0f, -16.0f), Vec4(-16.0f, -16.0f, -16.0f, -16.0f), Vec4(-16.0f, -16.0f, -16.0f, -16.0f), Vec4(-16.0f, -16.0f, -16.0f, -16.0f))));

      // Multiplication
      Mat4 m4mul = m4a * m4b;
      assert(mat4Equals(m4mul, Mat4(Vec4(250.0f, 260.0f, 270.0f, 280.0f), Vec4(618.0f, 644.0f, 670.0f, 696.0f), Vec4(986.0f, 1028.0f, 1070.0f, 1112.0f), Vec4(1354.0f, 1412.0f, 1470.0f, 1528.0f))));

      // Test scalar multiplication
      float scalar = 2.0f;
      Mat4 mScalarMul = m4a * scalar;
      assert(mat4Equals(mScalarMul, {Vec4(2.0f, 4.0f, 6.0f, 8.0f),
                                     Vec4(10.0f, 12.0f, 14.0f, 16.0f),
                                     Vec4(18.0f, 20.0f, 22.0f, 24.0f),
                                     Vec4(26.0f, 28.0f, 30.0f, 32.0f)}));

      // Test matrix-vector multiplication
      Vec4 vec4(1.0f, 2.0f, 3.0f, 4.0f);
      Vec4 m4vecmul = m4a * vec4;
      assert(vec4Equals(m4vecmul, Vec4(30.0f, 70.0f, 110.0f, 150.0f)));

      // Test matrix transpose
      Mat4 mTranspose4 = m4a.transpose();
      assert(mat4Equals(mTranspose4, {Vec4(1.0f, 5.0f, 9.0f, 13.0f),
                                      Vec4(2.0f, 6.0f, 10.0f, 14.0f),
                                      Vec4(3.0f, 7.0f, 11.0f, 15.0f),
                                      Vec4(4.0f, 8.0f, 12.0f, 16.0f)}));

      // Test matrix determinant
      float det4 = m4a.determinant();
      assert(det4 == 0.0f);
      float det42 = m4c.determinant();
      assert(det42 == 252.0f);

      // // Performance Test
      // clock_t begin = clock();
      // for (int i = 0; i < 100000; ++i)
      // {
      //   det42 = m4c.scalarDeterminant();
      // }
      // std::cout << "Scalar Performance: " << static_cast<double>(clock() - begin) << std::endl;
      // begin = clock();
      // for (int i = 0; i < 100000; ++i)
      // {
      //   det42 = m4c.determinant();
      // }
      // std::cout << "SSE Performance: " << static_cast<double>(clock() - begin) << std::endl
      //           << std::endl;

      // Test matrix inverse
      Mat4 mInverse4 = m4a.inverse();
      assert(mat4Equals(mInverse4, {Vec4f(0, 0, 0, 0),
                                    Vec4f(0, 0, 0, 0),
                                    Vec4f(0, 0, 0, 0),
                                    Vec4f(0, 0, 0, 0)}));
      // Doing this because I don't want to switch the values                                    
      Mat4 mInverse42 = m4c.inverse();
      assert(mat4Equals(mInverse42, {Vec4f(-0.23809523809523809478f, -0.19047619047619047692f, -0.33333333333333333472f, 0.38095238095238095362f),
                                     Vec4f(0.031746031746031746008f, -0.09126984126984126981f, 0.027777777777777777841f, 0.015873015873015872959f),
                                     Vec4f(-0.49206349206349206618f, 1.0396825396825396868f, 1.9444444444444444526f, -1.746031746031746039f),
                                     Vec4f(0.60317460317460317664f, -0.73412698412698413022f, -1.4722222222222222284f, 1.3015873015873015928f)}));
    }
  }
}

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
