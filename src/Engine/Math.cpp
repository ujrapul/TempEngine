// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Math.hpp"

namespace Temp::Math
{
  template struct Vec2<int>;
  template struct Vec2<unsigned int>;
  template struct Vec2<float>;

  template struct Vec3<float>;
  template struct Vec3<unsigned char>;

  template struct Vec4<float>;
  template struct Vec4<unsigned char>;

  // 2x2 matrix struct
  Mat2::Mat2() : rows{{1.0f, 0.0f}, {0.0f, 1.0f}} {}
  Mat2::Mat2(const Vec2f& col1, const Vec2f& col2) : rows{col1, col2} {}

  // Matrix-matrix addition
  Mat2 Mat2::operator+(const Mat2& other) const
  {
    return {rows[0] + other.rows[0], rows[1] + other.rows[1]};
  }

  // Matrix-matrix subtraction
  Mat2 Mat2::operator-(const Mat2& other) const
  {
    return {rows[0] - other.rows[0], rows[1] - other.rows[1]};
  }

  // Matrix-matrix multiplication using SIMD
  Mat2 Mat2::operator*(const Mat2& mat) const
  {
    // Mat2 result;

    // __m128 col0 = _mm_setr_ps(rows[0].x, rows[0].y, 0.0f, 0.0f);
    // __m128 col1 = _mm_setr_ps(rows[1].x, rows[1].y, 0.0f, 0.0f);
    // __m128 matCol0 = _mm_setr_ps(mat.rows[0].x, mat.rows[0].y, 0.0f, 0.0f);
    // __m128 matCol1 = _mm_setr_ps(mat.rows[1].x, mat.rows[1].y, 0.0f, 0.0f);

    // __m128 resCol0 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(col0, col0, _MM_SHUFFLE(0, 0, 0, 0)),
    // matCol0),
    //                             _mm_mul_ps(_mm_shuffle_ps(col0, col0, _MM_SHUFFLE(1, 1, 1, 1)),
    //                             matCol1));
    // __m128 resCol1 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(col1, col1, _MM_SHUFFLE(0, 0, 0, 0)),
    // matCol0),
    //                             _mm_mul_ps(_mm_shuffle_ps(col1, col1, _MM_SHUFFLE(1, 1, 1, 1)),
    //                             matCol1));

    // result.rows[0] = resCol0;
    // result.rows[1] = resCol1;

    // return result;

    Mat2 result{};

    result.rows[0] = Vec2f(rows[0].x * mat.rows[0].x + rows[0].y * mat.rows[1].x,
                           rows[0].x * mat.rows[0].y + rows[0].y * mat.rows[1].y);

    result.rows[1] = Vec2f(rows[1].x * mat.rows[0].x + rows[1].y * mat.rows[1].x,
                           rows[1].x * mat.rows[0].y + rows[1].y * mat.rows[1].y);

    return result;
  }

  // Matrix-float multiplication
  Mat2 Mat2::operator*(float scalar) const { return {rows[0] * scalar, rows[1] * scalar}; }

  // Matrix-vector multiplication
  Vec2f Mat2::operator*(const Vec2f& vec) const
  {
    // Vector based dot product
    return _mm_add_ps(_mm_mul_ps(vec.simdData, rows[0].simdData),
                      _mm_mul_ps(vec.simdData, rows[1].simdData));
  }

  bool Mat2::operator==(const Mat2& other) const
  {
    bool isEqual = true;
    for (int i = 0; i < 2; ++i)
    {
      isEqual &= rows[i] == other.rows[i];
    }
    return isEqual;
  }

  // Matrix transposition
  Mat2 Mat2::transpose() const
  {
    return {Vec2f(rows[0].x, rows[1].x), Vec2f(rows[0].y, rows[1].y)};
  }

  // Matrix determinant
  float Mat2::determinant() const
  {
    // __m128 a = rows[0].simdData;
    // __m128 b = rows[1].simdData;

    // __m128 det = _mm_sub_ps(_mm_mul_ps(a, _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 2, 0, 1))),
    //                         _mm_mul_ps(b, _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 2, 0, 1))));

    // float result;
    // _mm_store_ss(&result, det);
    // return result;
    return rows[0].x * rows[1].y - rows[0].y * rows[1].x;
  }

  // Matrix inverse
  Mat2 Mat2::inverse() const
  {
    float det = determinant();
    float invDet = 1.0f / (det + (det == 0.f)) * !(det == 0.f);

    return {Vec2f(rows[1].y, -rows[0].y) * invDet, Vec2f(-rows[1].x, rows[0].x) * invDet};
  }

  // 3x3 matrix struct
  Mat3::Mat3() : rows{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}} {}
  Mat3::Mat3(const Vec3f& col1, const Vec3f& col2, const Vec3f& col3) : rows{col1, col2, col3} {}

  // Matrix-matrix addition
  Mat3 Mat3::operator+(const Mat3& other) const
  {
    return {rows[0] + other.rows[0], rows[1] + other.rows[1], rows[2] + other.rows[2]};
  }

  // Matrix-matrix subtraction
  Mat3 Mat3::operator-(const Mat3& other) const
  {
    return {rows[0] - other.rows[0], rows[1] - other.rows[1], rows[2] - other.rows[2]};
  }

  // Matrix-matrix multiplication using SIMD
  Mat3 Mat3::operator*(const Mat3& mat) const
  {
    Mat3 result;

    __m128 col0 = _mm_setr_ps(rows[0].x, rows[0].y, rows[0].z, 0.0f);
    __m128 col1 = _mm_setr_ps(rows[1].x, rows[1].y, rows[1].z, 0.0f);
    __m128 col2 = _mm_setr_ps(rows[2].x, rows[2].y, rows[2].z, 0.0f);
    __m128 matCol0 = _mm_setr_ps(mat.rows[0].x, mat.rows[0].y, mat.rows[0].z, 0.0f);
    __m128 matCol1 = _mm_setr_ps(mat.rows[1].x, mat.rows[1].y, mat.rows[1].z, 0.0f);
    __m128 matCol2 = _mm_setr_ps(mat.rows[2].x, mat.rows[2].y, mat.rows[2].z, 0.0f);

    __m128 resCol0 = _mm_add_ps(
      _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(col0, col0, _MM_SHUFFLE(0, 0, 0, 0)), matCol0),
                 _mm_mul_ps(_mm_shuffle_ps(col0, col0, _MM_SHUFFLE(1, 1, 1, 1)), matCol1)),
      _mm_mul_ps(_mm_shuffle_ps(col0, col0, _MM_SHUFFLE(2, 2, 2, 2)), matCol2));
    __m128 resCol1 = _mm_add_ps(
      _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(col1, col1, _MM_SHUFFLE(0, 0, 0, 0)), matCol0),
                 _mm_mul_ps(_mm_shuffle_ps(col1, col1, _MM_SHUFFLE(1, 1, 1, 1)), matCol1)),
      _mm_mul_ps(_mm_shuffle_ps(col1, col1, _MM_SHUFFLE(2, 2, 2, 2)), matCol2));
    __m128 resCol2 = _mm_add_ps(
      _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(col2, col2, _MM_SHUFFLE(0, 0, 0, 0)), matCol0),
                 _mm_mul_ps(_mm_shuffle_ps(col2, col2, _MM_SHUFFLE(1, 1, 1, 1)), matCol1)),
      _mm_mul_ps(_mm_shuffle_ps(col2, col2, _MM_SHUFFLE(2, 2, 2, 2)), matCol2));

    result.rows[0] = resCol0;
    result.rows[1] = resCol1;
    result.rows[2] = resCol2;

    return result;
  }

  // Matrix-float multiplication
  Mat3 Mat3::operator*(float scalar) const
  {
    return {rows[0] * scalar, rows[1] * scalar, rows[2] * scalar};
  }

  // Matrix-vector multiplication
  Vec3f Mat3::operator*(const Vec3f& vec) const
  {
    // Load the vector into SIMD register
    __m128 vecSimd = vec.simdData;

    // Transpose the matrix
    __m128 row1 = _mm_load_ps(&rows[0].x);
    __m128 row2 = _mm_load_ps(&rows[1].x);
    __m128 row3 = _mm_load_ps(&rows[2].x);
    __m128 row4 = _mm_setzero_ps();
    _MM_TRANSPOSE4_PS(row1, row2, row3, row4);

    // Multiply the transposed matrix with the vector
    return _mm_add_ps(
      _mm_add_ps(_mm_mul_ps(row1, _mm_shuffle_ps(vecSimd, vecSimd, _MM_SHUFFLE(0, 0, 0, 0))),
                 _mm_mul_ps(row2, _mm_shuffle_ps(vecSimd, vecSimd, _MM_SHUFFLE(1, 1, 1, 1)))),
      _mm_mul_ps(row3, _mm_shuffle_ps(vecSimd, vecSimd, _MM_SHUFFLE(2, 2, 2, 2))));
  }

  bool Mat3::operator==(const Mat3& other) const
  {
    bool isEqual = true;
    for (int i = 0; i < 3; ++i)
    {
      isEqual &= rows[i] == other.rows[i];
    }
    return isEqual;
  }

  // Matrix transposition
  Mat3 Mat3::transpose() const
  {
    return {{rows[0].x, rows[1].x, rows[2].x},
            {rows[0].y, rows[1].y, rows[2].y},
            {rows[0].z, rows[1].z, rows[2].z}};
  }

  // Matrix determinant
  float Mat3::determinant() const
  {
    __m128 a = rows[0].simdData;
    __m128 b = rows[1].simdData;
    __m128 c = rows[2].simdData;

    __m128 det1 = _mm_mul_ps(_mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1)),
                             _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 1, 0, 2)));
    __m128 det2 = _mm_mul_ps(_mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2)),
                             _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1)));
    __m128 det = _mm_sub_ps(det1, det2);

    __m128 prod = _mm_mul_ps(a, det);
    float result;
    _mm_store_ss(&result, _mm_hadd_ps(prod, prod));

    return result;
  }

  // Matrix inverse
  Mat3 Mat3::inverse() const
  {
    float det = determinant();
    float invDet = 1.0f / (det + (det == 0.f)) * !(det == 0.f);

    Mat3 result;

    result.rows[0] = Vec3f((rows[1].y * rows[2].z - rows[1].z * rows[2].y) * invDet,
                           (rows[0].z * rows[2].y - rows[0].y * rows[2].z) * invDet,
                           (rows[0].y * rows[1].z - rows[0].z * rows[1].y) * invDet);

    result.rows[1] = Vec3f((rows[1].z * rows[2].x - rows[1].x * rows[2].z) * invDet,
                           (rows[0].x * rows[2].z - rows[0].z * rows[2].x) * invDet,
                           (rows[0].z * rows[1].x - rows[0].x * rows[1].z) * invDet);

    result.rows[2] = Vec3f((rows[1].x * rows[2].y - rows[1].y * rows[2].x) * invDet,
                           (rows[0].y * rows[2].x - rows[0].x * rows[2].y) * invDet,
                           (rows[0].x * rows[1].y - rows[0].y * rows[1].x) * invDet);

    return result;
  }

  void Mat3::print(const Mat3& mat3)
  {
    for (int i = 0; i < 3; ++i)
    {
      printf("[ %.4f %.4f %.4f ]\n", mat3.rows[i].x, mat3.rows[i].y, mat3.rows[i].z);
    }
  }

  // 4x4 matrix struct
  Mat4::Mat4()
      : rows{Vec4f{1.0f, 0.0f, 0.0f, 0.0f},
             Vec4f{0.0f, 1.0f, 0.0f, 0.0f},
             Vec4f{0.0f, 0.0f, 1.0f, 0.0f},
             Vec4f{0.0f, 0.0f, 0.0f, 1.0f}}
  {
  }

  Mat4::Mat4(const Vec4f& col1, const Vec4f& col2, const Vec4f& col3, const Vec4f& col4)
      : rows{col1, col2, col3, col4}
  {
  }

  // Matrix-matrix addition
  Mat4 Mat4::operator+(const Mat4& other) const
  {
    return {rows[0] + other.rows[0],
            rows[1] + other.rows[1],
            rows[2] + other.rows[2],
            rows[3] + other.rows[3]};
  }

  // Matrix-matrix subtraction
  Mat4 Mat4::operator-(const Mat4& other) const
  {
    return {rows[0] - other.rows[0],
            rows[1] - other.rows[1],
            rows[2] - other.rows[2],
            rows[3] - other.rows[3]};
  }

  // Matrix-matrix multiplication using SIMD
  Mat4 Mat4::operator*(const Mat4& mat) const
  {
    Mat4 result;

    // for (int i = 0; i < 4; ++i)
    // {
    //   __m128 row0 = _mm_set1_ps(rows[i].x);
    //   __m128 row1 = _mm_set1_ps(rows[i].y);
    //   __m128 row2 = _mm_set1_ps(rows[i].z);
    //   __m128 row3 = _mm_set1_ps(rows[i].w);

    //   __m128 prod0 = _mm_mul_ps(row0, mat.rows[0].simdData);
    //   __m128 prod1 = _mm_mul_ps(row1, mat.rows[1].simdData);
    //   __m128 prod2 = _mm_mul_ps(row2, mat.rows[2].simdData);
    //   __m128 prod3 = _mm_mul_ps(row3, mat.rows[3].simdData);

    //   __m128 sum0 = _mm_add_ps(prod0, prod1);
    //   __m128 sum1 = _mm_add_ps(prod2, prod3);
    //   __m128 sum2 = _mm_add_ps(sum0, sum1);

    //   result.rows[i].simdData = sum2;
    // }

    for (int i = 0; i < 4; ++i)
    {
      result.rows[i].x = rows[i].x * mat.rows[0].x + rows[i].y * mat.rows[1].x +
                         rows[i].z * mat.rows[2].x + rows[i].w * mat.rows[3].x;

      result.rows[i].y = rows[i].x * mat.rows[0].y + rows[i].y * mat.rows[1].y +
                         rows[i].z * mat.rows[2].y + rows[i].w * mat.rows[3].y;

      result.rows[i].z = rows[i].x * mat.rows[0].z + rows[i].y * mat.rows[1].z +
                         rows[i].z * mat.rows[2].z + rows[i].w * mat.rows[3].z;

      result.rows[i].w = rows[i].x * mat.rows[0].w + rows[i].y * mat.rows[1].w +
                         rows[i].z * mat.rows[2].w + rows[i].w * mat.rows[3].w;
    }

    return result;
  }

  // Matrix-float multiplication
  Mat4 Mat4::operator*(float scalar) const
  {
    return {rows[0] * scalar, rows[1] * scalar, rows[2] * scalar, rows[3] * scalar};
  }

  // Matrix-vector multiplication
  Vec4f Mat4::operator*(const Vec4f& vec) const
  {
    __m128 dot0 = _mm_mul_ps(rows[0].simdData, vec.simdData);
    __m128 dot1 = _mm_mul_ps(rows[1].simdData, vec.simdData);
    __m128 dot2 = _mm_mul_ps(rows[2].simdData, vec.simdData);
    __m128 dot3 = _mm_mul_ps(rows[3].simdData, vec.simdData);

    __m128 xy = _mm_hadd_ps(dot0, dot1);
    __m128 zw = _mm_hadd_ps(dot2, dot3);

    return _mm_hadd_ps(xy, zw);
  }

  const Vec4f& Mat4::operator[](int i) const { return rows[i]; }

  Vec4f& Mat4::operator[](int i) { return rows[i]; }

  bool Mat4::operator==(const Mat4& other) const
  {
    bool isEqual = true;
    for (int i = 0; i < 4; ++i)
    {
      isEqual &= rows[i] == other.rows[i];
    }
    return isEqual;
  }

  // Matrix transposition
  Mat4 Mat4::transpose() const
  {
    return {{rows[0].x, rows[1].x, rows[2].x, rows[3].x},
            {rows[0].y, rows[1].y, rows[2].y, rows[3].y},
            {rows[0].z, rows[1].z, rows[2].z, rows[3].z},
            {rows[0].w, rows[1].w, rows[2].w, rows[3].w}};
  }

  // Matrix determinant
  float Mat4::determinant() const
  {
    float a11 = rows[0].x, a12 = rows[0].y, a13 = rows[0].z, a14 = rows[0].w;
    float a21 = rows[1].x, a22 = rows[1].y, a23 = rows[1].z, a24 = rows[1].w;
    float a31 = rows[2].x, a32 = rows[2].y, a33 = rows[2].z, a34 = rows[2].w;
    float a41 = rows[3].x, a42 = rows[3].y, a43 = rows[3].z, a44 = rows[3].w;

    float det = a11 * (a22 * (a33 * a44 - a34 * a43) - a23 * (a32 * a44 - a34 * a42) +
                       a24 * (a32 * a43 - a33 * a42)) -
                a12 * (a21 * (a33 * a44 - a34 * a43) - a23 * (a31 * a44 - a34 * a41) +
                       a24 * (a31 * a43 - a33 * a41)) +
                a13 * (a21 * (a32 * a44 - a34 * a42) - a22 * (a31 * a44 - a34 * a41) +
                       a24 * (a31 * a42 - a32 * a41)) -
                a14 * (a21 * (a32 * a43 - a33 * a42) - a22 * (a31 * a43 - a33 * a41) +
                       a23 * (a31 * a42 - a32 * a41));

    return det;
  }

  // Matrix inverse
  Mat4 Mat4::inverse() const
  {
    const auto& data = rows;
    Mat4 inv;

    // Compute the matrix determinant
    float det = data[0][0] * (data[1][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) -
                              data[1][2] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) +
                              data[1][3] * (data[2][1] * data[3][2] - data[2][2] * data[3][1])) -
                data[0][1] * (data[1][0] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) -
                              data[1][2] * (data[2][0] * data[3][3] - data[2][3] * data[3][0]) +
                              data[1][3] * (data[2][0] * data[3][2] - data[2][2] * data[3][0])) +
                data[0][2] * (data[1][0] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) -
                              data[1][1] * (data[2][0] * data[3][3] - data[2][3] * data[3][0]) +
                              data[1][3] * (data[2][0] * data[3][1] - data[2][1] * data[3][0])) -
                data[0][3] * (data[1][0] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]) -
                              data[1][1] * (data[2][0] * data[3][2] - data[2][2] * data[3][0]) +
                              data[1][2] * (data[2][0] * data[3][1] - data[2][1] * data[3][0]));

    // Matrix is singular; cannot invert.
    if (det == 0.0f)
    {
      return inv * 0;
    }

    float inv_det = 1.0f / det;

    // Compute the inverse matrix elements
    inv[0][0] = inv_det * (data[1][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) -
                           data[1][2] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) +
                           data[1][3] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]));
    inv[0][1] = inv_det * -(data[0][1] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) -
                            data[0][2] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) +
                            data[0][3] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]));
    inv[0][2] = inv_det * (data[0][1] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) -
                           data[0][2] * (data[1][1] * data[3][3] - data[1][3] * data[3][1]) +
                           data[0][3] * (data[1][1] * data[3][2] - data[1][2] * data[3][1]));
    inv[0][3] = inv_det * -(data[0][1] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]) -
                            data[0][2] * (data[1][1] * data[2][3] - data[1][3] * data[2][1]) +
                            data[0][3] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]));

    inv[1][0] = inv_det * -(data[1][0] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) -
                            data[1][2] * (data[2][0] * data[3][3] - data[2][3] * data[3][0]) +
                            data[1][3] * (data[2][0] * data[3][2] - data[2][2] * data[3][0]));
    inv[1][1] = inv_det * (data[0][0] * (data[2][2] * data[3][3] - data[2][3] * data[3][2]) -
                           data[0][2] * (data[2][0] * data[3][3] - data[2][3] * data[3][0]) +
                           data[0][3] * (data[2][0] * data[3][2] - data[2][2] * data[3][0]));
    inv[1][2] = inv_det * -(data[0][0] * (data[1][2] * data[3][3] - data[1][3] * data[3][2]) -
                            data[0][2] * (data[1][0] * data[3][3] - data[1][3] * data[3][0]) +
                            data[0][3] * (data[1][0] * data[3][2] - data[1][2] * data[3][0]));
    inv[1][3] = inv_det * (data[0][0] * (data[1][2] * data[2][3] - data[1][3] * data[2][2]) -
                           data[0][2] * (data[1][0] * data[2][3] - data[1][3] * data[2][0]) +
                           data[0][3] * (data[1][0] * data[2][2] - data[1][2] * data[2][0]));

    inv[2][0] = inv_det * (data[1][0] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) -
                           data[1][1] * (data[2][0] * data[3][3] - data[2][3] * data[3][0]) +
                           data[1][3] * (data[2][0] * data[3][1] - data[2][1] * data[3][0]));
    inv[2][1] = inv_det * -(data[0][0] * (data[2][1] * data[3][3] - data[2][3] * data[3][1]) -
                            data[0][1] * (data[2][0] * data[3][3] - data[2][3] * data[3][0]) +
                            data[0][3] * (data[2][0] * data[3][1] - data[2][1] * data[3][0]));
    inv[2][2] = inv_det * (data[0][0] * (data[1][1] * data[3][3] - data[1][3] * data[3][1]) -
                           data[0][1] * (data[1][0] * data[3][3] - data[1][3] * data[3][0]) +
                           data[0][3] * (data[1][0] * data[3][1] - data[1][1] * data[3][0]));
    inv[2][3] = inv_det * -(data[0][0] * (data[1][1] * data[2][3] - data[1][3] * data[2][1]) -
                            data[0][1] * (data[1][0] * data[2][3] - data[1][3] * data[2][0]) +
                            data[0][3] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]));

    inv[3][0] = inv_det * -(data[1][0] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]) -
                            data[1][1] * (data[2][0] * data[3][2] - data[2][2] * data[3][0]) +
                            data[1][2] * (data[2][0] * data[3][1] - data[2][1] * data[3][0]));
    inv[3][1] = inv_det * (data[0][0] * (data[2][1] * data[3][2] - data[2][2] * data[3][1]) -
                           data[0][1] * (data[2][0] * data[3][2] - data[2][2] * data[3][0]) +
                           data[0][2] * (data[2][0] * data[3][1] - data[2][1] * data[3][0]));
    inv[3][2] = inv_det * -(data[0][0] * (data[1][1] * data[3][2] - data[1][2] * data[3][1]) -
                            data[0][1] * (data[1][0] * data[3][2] - data[1][2] * data[3][0]) +
                            data[0][2] * (data[1][0] * data[3][1] - data[1][1] * data[3][0]));
    inv[3][3] = inv_det * (data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]) -
                           data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0]) +
                           data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]));

    return inv;
  }

  Mat4 Mat4::translate(const Vec3f& translation)
  {
    Mat4 translateMatrix;
    translateMatrix[0].w = translation.x;
    translateMatrix[1].w = translation.y;
    translateMatrix[2].w = translation.z;

    return *this * translateMatrix;
  }

  void Mat4::setTranslation(const Vec3f& translation)
  {
    (*this)[0].w = translation.x;
    (*this)[1].w = translation.y;
    (*this)[2].w = translation.z;
  }

  Mat4 Mat4::scale(const Vec3f& scale)
  {
    Mat4 scaleMatrix;
    scaleMatrix[0].x = scale.x;
    scaleMatrix[1].y = scale.y;
    scaleMatrix[2].z = scale.z;

    return *this * scaleMatrix;
  }

  void Mat4::setScale(const Vec3f& scale)
  {
    (*this)[0].x = scale.x;
    (*this)[1].y = scale.y;
    (*this)[2].z = scale.z;
  }

  // Rotations take radians!

  Mat4 Mat4::rotateX(float angle)
  {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    Mat4 rotationMatrix;
    rotationMatrix[1][1] = cosAngle;
    rotationMatrix[1][2] = -sinAngle;
    rotationMatrix[2][1] = sinAngle;
    rotationMatrix[2][2] = cosAngle;

    return *this * rotationMatrix;
  }

  Mat4 Mat4::rotateY(float angle)
  {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    Mat4 rotationMatrix;
    rotationMatrix[0][0] = cosAngle;
    rotationMatrix[0][2] = sinAngle;
    rotationMatrix[2][0] = -sinAngle;
    rotationMatrix[2][2] = cosAngle;

    return *this * rotationMatrix;
  }

  Mat4 Mat4::rotateZ(float angle)
  {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    Mat4 rotationMatrix;
    rotationMatrix[0][0] = cosAngle;
    rotationMatrix[0][1] = -sinAngle;
    rotationMatrix[1][0] = sinAngle;
    rotationMatrix[1][1] = cosAngle;

    return *this * rotationMatrix;
  }

  Mat4 Mat4::rotate(float angle, const Vec3f& axis)
  {
    Mat4 mat;
    float c = std::cos(angle);
    float s = std::sin(angle);
    float t = 1.0f - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    // Normalize the axis
    float length = std::sqrt(x * x + y * y + z * z);
    if (length != 0.0f)
    {
      x /= length;
      y /= length;
      z /= length;
    }

    // Calculate the rotation matrix elements
    float xt = x * t;
    float yt = y * t;
    float zt = z * t;
    float xs = x * s;
    float ys = y * s;
    float zs = z * s;

    // Set the rotation matrix elements
    mat[0][0] = xt * x + c;
    mat[0][1] = xt * y + zs;
    mat[0][2] = xt * z - ys;
    mat[0][3] = 0.0f;

    mat[1][0] = xt * y - zs;
    mat[1][1] = yt * y + c;
    mat[1][2] = yt * z + xs;
    mat[1][3] = 0.0f;

    mat[2][0] = xt * z + ys;
    mat[2][1] = yt * z - xs;
    mat[2][2] = zt * z + c;
    mat[2][3] = 0.0f;

    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;

    return *this * mat;
  }

  // Need to underscore to avoid conflict with near and far defines in windows
  Mat4 Mat4::perspective(float fov, float aspectRatio, float _near, float _far)
  {
    Mat4 result;

    float tanHalfFOV = tanf(fov / 2.0f);
    float range = _near - _far;

    result[0][0] = 1.0f / (aspectRatio * tanHalfFOV);
    result[1][1] = 1.0f / tanHalfFOV;
    result[2][2] = (-_near - _far) / range;
    result[2][3] = 2.0f * _far * _near / range;
    result[3][2] = 1.0f;

    return result;
  }

  Mat4 Mat4::orthographic(float left, float right, float bottom, float top, float _near, float _far)
  {
    Mat4 result;

    float width = right - left;
    float height = top - bottom;
    float depth = _far - _near;

    result[0][0] = 2.0f / width;
    result[1][1] = 2.0f / height;
    result[2][2] = -2.0f / depth;
    result[0][3] = -(right + left) / width;
    result[1][3] = -(top + bottom) / height;
    result[2][3] = -(_far + _near) / depth;
    result[3][3] = 1.0f;

    return result;
  }

  bool RayTriangleIntersect(const Vec3f& rayOrigin,
                            const Vec3f& rayDirection,
                            const Vec3f& v0,
                            const Vec3f& v1,
                            const Vec3f& v2,
                            Vec3f& intersectionPoint)
  {
    const float EPSILON = 0.000001f;
    // std::cout << rayOrigin << " " << rayDirection << " " << v0 << " " << v1 << " " << v2 <<
    // std::endl; Calculate the triangle's normal
    Vec3f edge1 = v1 - v0;
    Vec3f edge2 = v2 - v0;
    // Vec3f normal = edge1.cross(edge2);

    // begin calculating determinant - also used to calculate U parameter
    auto pvec = rayDirection.cross(edge2);

    // if determinant is near zero, ray lies in plane of triangle
    auto det = edge1.dot(pvec);

    if (det > -EPSILON && det < EPSILON)
      return false;
    float inv_det = 1.0f / det;

    // calculate distance from vert to ray origin
    auto tvec = rayOrigin - v0;

    // calculate U parameter and test bounds
    float u = tvec.dot(pvec) * inv_det;
    if (u < 0.0f || u > 1.0f)
    {
      return false;
    }

    // prepare to test V parameter
    auto qvec = tvec.cross(edge1);

    // calculate V parameter and test bounds
    float v = rayDirection.dot(qvec) * inv_det;
    if (v < 0.0 || u + v > 1.0)
    {
      return false;
    }

    // calculate t, ray intersects triangle
    float t = edge2.dot(qvec) * inv_det;
    if (t < 0)
    {
      return false;
    }

    intersectionPoint = {t, u, v};

    // std::cout << intersectionPoint << std::endl;
    return true; // Intersection point is inside the triangle
  }

  bool RayMeshIntersect(const Vec3f& rayOrigin,
                        const Vec3f& rayDirection,
                        const SceneDynamicArray<SceneDynamicArray<Vec3f>>& triangles,
                        Vec3f& intersectionPoint)
  {
    for (const auto& triangle : triangles)
    {
      if (RayTriangleIntersect(rayOrigin,
                               rayDirection,
                               triangle[0],
                               triangle[1],
                               triangle[2],
                               intersectionPoint))
      {
        return true;
      }
    }
    return false;
  }
}

template struct Temp::DynamicArray<Temp::Math::Vec3<float>, Temp::MemoryManager::Data::SCENE_ARENA>;
template struct Temp::DynamicArray<Temp::DynamicArray<Temp::Math::Vec3<float>, Temp::MemoryManager::Data::SCENE_ARENA>>;

/// Previous versions of inverse:

// // Matrix determinant
// float determinant() const
// {
//   // float a11 = rows[0].x, a12 = rows[0].y, a13 = rows[0].z, a14 = rows[0].w;
//   // float a21 = rows[1].x, a22 = rows[1].y, a23 = rows[1].z, a24 = rows[1].w;
//   // float a31 = rows[2].x, a32 = rows[2].y, a33 = rows[2].z, a34 = rows[2].w;
//   // float a41 = rows[3].x, a42 = rows[3].y, a43 = rows[3].z, a44 = rows[3].w;

//   __m128 a3332 = _mm_set_ps(rows[2].y, rows[2].y, rows[2].z, rows[2].z);
//   __m128 a4443 = _mm_set_ps(rows[3].z, rows[3].w, rows[3].w, rows[3].w);
//   __m128 a3433 = _mm_set_ps(rows[2].z, rows[2].w, rows[2].w, rows[2].w);
//   __m128 a4342 = _mm_set_ps(rows[3].y, rows[3].y, rows[3].z, rows[3].z);
//   __m128 a3231 = _mm_set_ps(rows[2].x, rows[2].x, rows[2].x, rows[2].y);
//   __m128 a4241 = _mm_set_ps(rows[3].x, rows[3].x, rows[3].x, rows[3].y);

//   __m128 detM0 = _mm_sub_ps(_mm_mul_ps(a3332, a4443), _mm_mul_ps(a3433, a4342));
//   __m128 detM1 = _mm_set_ps(rows[1].x, rows[1].x, rows[1].x, rows[1].y);
//   __m128 detM10 = _mm_mul_ps(detM0, detM1);

//   __m128 detM02 = _mm_sub_ps(_mm_mul_ps(a3231, a4443), _mm_mul_ps(a3433, a4241));
//   __m128 detM2 = _mm_set_ps(rows[1].y, rows[1].y, rows[1].z, rows[1].z);
//   __m128 detM20 = _mm_mul_ps(detM02, detM2);

//   __m128 detM03 = _mm_sub_ps(_mm_mul_ps(a3231, a4342), _mm_mul_ps(a3332, a4241));
//   __m128 detM3 = _mm_set_ps(rows[1].z, rows[1].w, rows[1].w, rows[1].w);
//   __m128 detM30 = _mm_mul_ps(detM03, detM3);

//   __m128 detM40 = _mm_add_ps(_mm_sub_ps(detM10, detM20), detM30);

//   __m128 xmm0 = _mm_mul_ps(rows[0].simdData, detM40);

//   __m128 det = _mm_hadd_ps(_mm_hsub_ps(xmm0, xmm0), _mm_hsub_ps(xmm0, xmm0));

//   // float det =
//   //     a11 * (a22 * (a33 * a44 - a34 * a43) - a23 * (a32 * a44 - a34 * a42) + a24 * (a32 *
//   a43
//   - a33 * a42)) -
//   //     a12 * (a21 * (a33 * a44 - a34 * a43) - a23 * (a31 * a44 - a34 * a41) + a24 * (a31 *
//   a43
//   - a33 * a41)) +
//   //     a13 * (a21 * (a32 * a44 - a34 * a42) - a22 * (a31 * a44 - a34 * a41) + a24 * (a31 *
//   a42
//   - a32 * a41)) -
//   //     a14 * (a21 * (a32 * a43 - a33 * a42) - a22 * (a31 * a43 - a33 * a41) + a23 * (a31 *
//   a42
//   - a32 * a41));

//   float result;
//   _mm_store_ss(&result, det);
//   return result;
// }

// // Calculate the matrix of minors
// float m11 = rows[1].y * (rows[2].z * rows[3].w - rows[2].w * rows[3].z) -
//             rows[1].z * (rows[2].y * rows[3].w - rows[2].w * rows[3].y) +
//             rows[1].w * (rows[2].y * rows[3].z - rows[2].z * rows[3].y);

// float m12 = rows[1].x * (rows[2].z * rows[3].w - rows[2].w * rows[3].z) -
//             rows[1].z * (rows[2].x * rows[3].w - rows[2].w * rows[3].x) +
//             rows[1].w * (rows[2].x * rows[3].z - rows[2].z * rows[3].x);

// float m13 = rows[1].x * (rows[2].y * rows[3].w - rows[2].w * rows[3].y) -
//             rows[1].y * (rows[2].x * rows[3].w - rows[2].w * rows[3].x) +
//             rows[1].w * (rows[2].x * rows[3].y - rows[2].y * rows[3].x);

// float m14 = rows[1].x * (rows[2].y * rows[3].z - rows[2].z * rows[3].y) -
//             rows[1].y * (rows[2].x * rows[3].z - rows[2].z * rows[3].x) +
//             rows[1].z * (rows[2].x * rows[3].y - rows[2].y * rows[3].x);

// float m21 = rows[0].y * (rows[2].z * rows[3].w - rows[2].w * rows[3].z) -
//             rows[0].z * (rows[2].y * rows[3].w - rows[2].w * rows[3].y) +
//             rows[0].w * (rows[2].y * rows[3].z - rows[2].z * rows[3].y);

// float m22 = rows[0].x * (rows[2].z * rows[3].w - rows[2].w * rows[3].z) -
//             rows[0].z * (rows[2].x * rows[3].w - rows[2].w * rows[3].x) +
//             rows[0].w * (rows[2].x * rows[3].z - rows[2].z * rows[3].x);

// float m23 = rows[0].x * (rows[2].y * rows[3].w - rows[2].w * rows[3].y) -
//             rows[0].y * (rows[2].x * rows[3].w - rows[2].w * rows[3].x) +
//             rows[0].w * (rows[2].x * rows[3].y - rows[2].y * rows[3].x);

// float m24 = rows[0].x * (rows[2].y * rows[3].z - rows[2].z * rows[3].y) -
//             rows[0].y * (rows[2].x * rows[3].z - rows[2].z * rows[3].x) +
//             rows[0].z * (rows[2].x * rows[3].y - rows[2].y * rows[3].x);

// float m31 = rows[0].y * (rows[1].z * rows[3].w - rows[1].w * rows[3].z) -
//             rows[0].z * (rows[1].y * rows[3].w - rows[1].w * rows[3].y) +
//             rows[0].w * (rows[1].y * rows[3].z - rows[1].z * rows[3].y);

// float m32 = rows[0].x * (rows[1].z * rows[3].w - rows[1].w * rows[3].z) -
//             rows[0].z * (rows[1].x * rows[3].w - rows[1].w * rows[3].x) +
//             rows[0].w * (rows[1].x * rows[3].z - rows[1].z * rows[3].x);

// float m33 = rows[0].x * (rows[1].y * rows[3].w - rows[1].w * rows[3].y) -
//             rows[0].y * (rows[1].x * rows[3].w - rows[1].w * rows[3].x) +
//             rows[0].w * (rows[1].x * rows[3].y - rows[1].y * rows[3].x);

// float m34 = rows[0].x * (rows[1].y * rows[3].z - rows[1].z * rows[3].y) -
//             rows[0].y * (rows[1].x * rows[3].z - rows[1].z * rows[3].x) +
//             rows[0].z * (rows[1].x * rows[3].y - rows[1].y * rows[3].x);

// float m41 = rows[0].y * (rows[1].z * rows[2].w - rows[1].w * rows[2].z) -
//             rows[0].z * (rows[1].y * rows[2].w - rows[1].w * rows[2].y) +
//             rows[0].w * (rows[1].y * rows[2].z - rows[1].z * rows[2].y);

// float m42 = rows[0].x * (rows[1].z * rows[2].w - rows[1].w * rows[2].z) -
//             rows[0].z * (rows[1].x * rows[2].w - rows[1].w * rows[2].x) +
//             rows[0].w * (rows[1].x * rows[2].z - rows[1].z * rows[2].x);

// float m43 = rows[0].x * (rows[1].y * rows[2].w - rows[1].w * rows[2].y) -
//             rows[0].y * (rows[1].x * rows[2].w - rows[1].w * rows[2].x) +
//             rows[0].w * (rows[1].x * rows[2].y - rows[1].y * rows[2].x);

// float m44 = rows[0].x * (rows[1].y * rows[2].z - rows[1].z * rows[2].y) -
//             rows[0].y * (rows[1].x * rows[2].z - rows[1].z * rows[2].x) +
//             rows[0].z * (rows[1].x * rows[2].y - rows[1].y * rows[2].x);

// // Calculate the determinant of the matrix
// float det = rows[0].x * m11 - rows[0].y * m12 + rows[0].z * m13 - rows[0].w * m14;
// std::cout << rows[0].x * m11 << " " << rows[0].y * m12 << " " << rows[0].z * m13 << " " <<
// rows[0].w * m14 << std::endl; if (Math::Abs(det) < 0.000001f)
// {
//   return {};
// }

// // float invDet = 1.0f / (det + (Math::Abs(det) < 0.000001f)) * !(Math::Abs(det) < 0.000001f);
// float invDet = 1.0f / det;

// // Calculate the adjugate matrix
// Mat4 adjugate(
//     Vec4(m11, -m21, m31, -m41),
//     Vec4(-m12, m22, -m32, m42),
//     Vec4(m13, -m23, m33, -m43),
//     Vec4(-m14, m24, -m34, m44));

// // Multiply the adjugate matrix by the inverse determinant to get the inverse matrix
// return adjugate * invDet;

// auto m = rows;
// float SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
// float SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
// float SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
// float SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
// float SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
// float SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
// float SubFactor06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
// float SubFactor07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
// float SubFactor08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
// float SubFactor09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
// float SubFactor10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
// float SubFactor11 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
// float SubFactor12 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
// float SubFactor13 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
// float SubFactor14 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
// float SubFactor15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
// float SubFactor16 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
// float SubFactor17 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

// Mat4 Inverse;
// Inverse[0][0] = +(m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02);
// Inverse[0][1] = -(m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04);
// Inverse[0][2] = +(m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05);
// Inverse[0][3] = -(m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05);

// Inverse[1][0] = -(m[0][1] * SubFactor00 - m[0][2] * SubFactor01 + m[0][3] * SubFactor02);
// Inverse[1][1] = +(m[0][0] * SubFactor00 - m[0][2] * SubFactor03 + m[0][3] * SubFactor04);
// Inverse[1][2] = -(m[0][0] * SubFactor01 - m[0][1] * SubFactor03 + m[0][3] * SubFactor05);
// Inverse[1][3] = +(m[0][0] * SubFactor02 - m[0][1] * SubFactor04 + m[0][2] * SubFactor05);

// Inverse[2][0] = +(m[0][1] * SubFactor06 - m[0][2] * SubFactor07 + m[0][3] * SubFactor08);
// Inverse[2][1] = -(m[0][0] * SubFactor06 - m[0][2] * SubFactor09 + m[0][3] * SubFactor10);
// Inverse[2][2] = +(m[0][0] * SubFactor07 - m[0][1] * SubFactor09 + m[0][3] * SubFactor11);
// Inverse[2][3] = -(m[0][0] * SubFactor08 - m[0][1] * SubFactor10 + m[0][2] * SubFactor11);

// Inverse[3][0] = -(m[0][1] * SubFactor12 - m[0][2] * SubFactor13 + m[0][3] * SubFactor14);
// Inverse[3][1] = +(m[0][0] * SubFactor12 - m[0][2] * SubFactor15 + m[0][3] * SubFactor16);
// Inverse[3][2] = -(m[0][0] * SubFactor13 - m[0][1] * SubFactor15 + m[0][3] * SubFactor17);
// Inverse[3][3] = +(m[0][0] * SubFactor14 - m[0][1] * SubFactor16 + m[0][2] * SubFactor17);

// float Determinant =
//     +m[0][0] * Inverse[0][0] + m[0][1] * Inverse[0][1] + m[0][2] * Inverse[0][2] + m[0][3] *
//     Inverse[0][3];

// if (Determinant == 0)
// {
//   return Mat4() * 0;
// }

// Inverse = Inverse * (1.0 / Determinant);

// return Inverse.transpose();
