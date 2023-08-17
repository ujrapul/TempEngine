// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Math.hpp"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include "gl.h"
#endif

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Engine
{
  struct Data;
}

namespace Temp::Camera
{
  enum class Projection
  {
    PERSPECTIVE,
    ORTHOGRAPHIC
  };

  GLuint UBO();
  GLuint FontUBO();
  void UpdateOrthoScale(Scene::Data& scene, float _orthoScale);
  void UpdateFontOrthoScale(Scene::Data& scene, float _fontOrthoScale);
  void UpdateCameraAspect(Scene::Data& scene, float _width, float _height);
  float GetAspect();
  float GetHeight();
  float GetWidth();
  void UpdateFov(Scene::Data& scene, float _fov);
  void SetProjection(Projection _projection);
  void TranslateView(const Math::Vec3f& translate);
  void ResetView();
  void Drag(float lastX, float lastY, float currX, float currY);
  const Math::Mat4& GetOrthoProjecton();
  const Math::Mat4& GetView();
  Math::Vec2f ConvertToFontOrthoViewSpace(float x, float y);
  Math::Vec2f ConvertToOrthoViewSpace(float x, float y);
  Math::Vec2f ConvertToNDC(float x, float y);
  Math::Vec4f ConvertNDCToClipSpace(float x, float y);
  Math::Vec4f ConvertClipToViewSpace(const Math::Vec4f clipCoords);
  Math::Vec4f ConvertClipToFontViewSpace(const Math::Vec4f clipCoords);
  Math::Vec4f ConvertScreenCoordsToViewSpace(float x, float y);
  Math::Vec4f ConvertScreenCoordsToFontViewSpace(float x, float y);
}
