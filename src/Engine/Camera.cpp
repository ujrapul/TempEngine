// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Camera.hpp"
#include "Engine.hpp"
#include "Scene.hpp"

namespace Temp::Camera
{
  namespace
  {
    float orthoScale{1.f};
    float fontOrthoScale{1.f};
    float internalScale{1.f};
    float finalOrthoScale{1.f};
    float finalFontOrthoScale{1.f};
    float aspect{1280.f / 720.f};
    float width{1280};
    float height{720};
    float fov{45.f};

    Math::Mat4 FontOrthoProjection()
    {
      return Math::Mat4::orthographic(-width / 2 * fontOrthoScale * internalScale,
                                      width / 2 * fontOrthoScale * internalScale,
                                      -height / 2 * fontOrthoScale * internalScale,
                                      height / 2 * fontOrthoScale * internalScale,
                                      -100,
                                      100);
    }

    Math::Mat4 OrthoProjection()
    {
      return Math::Mat4::orthographic(-width / 2 * orthoScale * internalScale,
                                      width / 2 * orthoScale * internalScale,
                                      -height / 2 * orthoScale * internalScale,
                                      height / 2 * orthoScale * internalScale,
                                      -100,
                                      100);
    }

    Projection projection = Projection::PERSPECTIVE;

    Math::Mat4 orthoProjection = OrthoProjection();
    Math::Mat4 fontOrthoProjection = FontOrthoProjection();

    Math::Mat4 perspProjection = Math::Mat4::perspective(Math::ToRadians(fov), aspect, 0.1f, 100.f);

    Math::Mat4 view{};
    Math::Mat4 fontView{};

    Math::Mat4& GetProjection()
    {
      switch (projection)
      {
        case Projection::PERSPECTIVE:
          return perspProjection;
        default:
          return orthoProjection;
      };
    }

    const Math::Mat4& GetProjection(Projection _projection)
    {
      switch (_projection)
      {
        case Projection::PERSPECTIVE:
          return perspProjection;
        default:
          return orthoProjection;
      };
    }

    void UpdateCamera()
    {
      using namespace Temp::Render;

      // Matrices need to be transposed since OpenGL uses column major matrices
      OpenGLWrapper::UpdateUBO(UBO(),
                               &GetProjection().transpose().rows[0][0],
                               sizeof(Math::Mat4),
                               0);
      OpenGLWrapper::UpdateUBO(FontUBO(),
                               &fontOrthoProjection.transpose().rows[0][0],
                               sizeof(Math::Mat4),
                               0);
      OpenGLWrapper::UpdateUBO(UBO(),
                               &view.transpose().rows[0][0],
                               sizeof(Math::Mat4),
                               sizeof(Math::Mat4));
      OpenGLWrapper::UpdateUBO(FontUBO(),
                               &fontView.transpose().rows[0][0],
                               sizeof(Math::Mat4),
                               sizeof(Math::Mat4));
      OpenGLWrapper::UpdateUBO(UBO(), //
                               &finalOrthoScale,
                               sizeof(float),
                               sizeof(Math::Mat4) * 2);
      OpenGLWrapper::UpdateUBO(FontUBO(),
                               &finalFontOrthoScale,
                               sizeof(float),
                               sizeof(Math::Mat4) * 2);
      OpenGLWrapper::UpdateUBO(UBO(),
                               &width,
                               sizeof(float),
                               sizeof(Math::Mat4) * 2 + 1 * sizeof(float));
      OpenGLWrapper::UpdateUBO(FontUBO(),
                               &width,
                               sizeof(float),
                               sizeof(Math::Mat4) * 2 + 1 * sizeof(float));
      OpenGLWrapper::UpdateUBO(UBO(),
                               &height,
                               sizeof(float),
                               sizeof(Math::Mat4) * 2 + 2 * sizeof(float));
      OpenGLWrapper::UpdateUBO(FontUBO(),
                               &height,
                               sizeof(float),
                               sizeof(Math::Mat4) * 2 + 2 * sizeof(float));
    }

    void UpdateCamera(Scene::Data& /*scene*/, void* /*data*/) { UpdateCamera(); }
  }

  GLuint UBO()
  {
    // 128 bytes because this UBO contains the View and Projection matrices
    static GLuint UBO = Render::OpenGLWrapper::CreateUBO(2 * sizeof(Math::Mat4) +
                                                         3 * sizeof(float));
    return UBO;
  }

  GLuint FontUBO()
  {
    // 128 bytes because this UBO contains the View and Projection matrices
    static GLuint FontUBO = Render::OpenGLWrapper::CreateUBO(2 * sizeof(Math::Mat4) +
                                                             3 * sizeof(float));
    return FontUBO;
  }

  void UpdateOrthoScale(Scene::Data& scene, float _orthoScale)
  {
    orthoScale = _orthoScale;
    finalOrthoScale = orthoScale * internalScale;
    orthoProjection = OrthoProjection();
    Temp::Scene::EnqueueRender(scene, UpdateCamera, nullptr);
  }

  void UpdateFontOrthoScale(Scene::Data& scene, float _orthoScale)
  {
    fontOrthoScale = _orthoScale;
    finalFontOrthoScale = fontOrthoScale * internalScale;
    fontOrthoProjection = FontOrthoProjection();
    Temp::Scene::EnqueueRender(scene, UpdateCamera, nullptr);
  }

  void UpdateCameraAspect(Scene::Data& scene, float _width, float _height)
  {
    width = _width;
    height = _height;
    aspect = width / height;
    orthoProjection = OrthoProjection();
    fontOrthoProjection = FontOrthoProjection();
    perspProjection = Math::Mat4::perspective(Math::ToRadians(fov), aspect, 0.1f, 100.f);
    Temp::Scene::EnqueueRender(scene, UpdateCamera, nullptr);
  }

  float GetAspect() { return aspect; }

  float GetHeight() { return height; }

  float GetWidth() { return width; }

  void UpdateFov(Scene::Data& scene, float _fov)
  {
    fov = _fov;
    perspProjection = Math::Mat4::perspective(Math::ToRadians(fov), aspect, 0.1f, 100.f);
    Temp::Scene::EnqueueRender(scene, UpdateCamera, nullptr);
  }

  // Should only be called once!
  void SetProjection(Projection _projection) { projection = _projection; }

  void TranslateView(const Math::Vec3f& translate) { view = view.translate(translate); }

  void ResetView() { view = {}; }

  void Drag(float lastX, float lastY, float currX, float currY)
  {
    switch (projection)
    {
      case Projection::PERSPECTIVE:
        break;
      case Projection::ORTHOGRAPHIC:
      {
        auto currViewSpaceCoords = ConvertScreenCoordsToViewSpace(currX, currY);
        auto lastViewSpaceCoords = ConvertScreenCoordsToViewSpace(lastX, lastY);
        view[0].w += (currViewSpaceCoords.x - lastViewSpaceCoords.x);
        view[1].w += (currViewSpaceCoords.y - lastViewSpaceCoords.y);
        fontView[0].w += (currViewSpaceCoords.x - lastViewSpaceCoords.x);
        fontView[1].w += (currViewSpaceCoords.y - lastViewSpaceCoords.y);
        UpdateCamera();

        // std::cout << currViewSpaceCoords << " " << lastViewSpaceCoords << std::endl;
      }
      break;
      default:
        break;
    };
  }

  const Math::Mat4& GetOrthoProjecton() { return GetProjection(Projection::ORTHOGRAPHIC); }

  const Math::Mat4& GetView() { return view; }

  Math::Vec2f ConvertToFontOrthoViewSpace(float x, float y)
  {
    float normalizedX = x / width;
    float normalizedY = y / height;

    return {(-width / 2 + width * normalizedX) * fontOrthoScale,
            (height / 2 - height * normalizedY) * fontOrthoScale};
  }

  Math::Vec2f ConvertToOrthoViewSpace(float x, float y)
  {
    float normalizedX = x / width;
    float normalizedY = y / height;

    return {(-width / 2 + width * normalizedX) * orthoScale,
            (height / 2 - height * normalizedY) * orthoScale};
  }

  Math::Vec2f ConvertToNDC(float x, float y)
  {
    // Convert the mouse position to normalized device coordinates (NDC)
    float ndcX = (2.0f * x) / width - 1.0f;
    float ndcY = 1.0f - (2.0f * y) / height;

    return {ndcX, ndcY};
  }

  Math::Vec4f ConvertNDCToClipSpace(float ndcX, float ndcY)
  {
    // Convert NDC to clip space
    Math::Vec4f clipCoords(ndcX, ndcY, -1.0f, 1.0f); // Assume the mouse position is at depth -1
    Math::Mat4 inverseProjectionMatrix = GetProjection().inverse();
    // std::cout << inverseProjectionMatrix << std::endl;
    // std::cout << inverseProjectionMatrix << std::endl;
    Math::Vec4f eyeCoords = inverseProjectionMatrix * clipCoords;
    // std::cout << clipCoords << std::endl;
    // std::cout << eyeCoords << std::endl;
    return eyeCoords / eyeCoords.w;
  }

  Math::Vec4f ConvertClipToViewSpace(const Math::Vec4f clipCoords)
  {
    Math::Mat4 inverseViewMatrix = view.inverse();
    // std::cout << inverseViewMatrix << std::endl;
    // Convert view space (coordinates in world)
    return inverseViewMatrix * clipCoords;
  }

  Math::Vec4f ConvertClipToFontViewSpace(const Math::Vec4f clipCoords)
  {
    Math::Mat4 inverseViewMatrix = fontView.inverse();
    // std::cout << inverseViewMatrix << std::endl;
    // Convert view space (coordinates in world)
    return inverseViewMatrix * clipCoords;
  }

  Math::Vec4f ConvertScreenCoordsToViewSpace(float x, float y)
  {
    auto ndc = ConvertToNDC(x, y);
    auto clipCoords = ConvertNDCToClipSpace(ndc.x, ndc.y);
    auto worldCoords = ConvertClipToViewSpace(clipCoords);
    // std::cout << worldCoords << std::endl;
    return worldCoords;
  }

  Math::Vec4f ConvertScreenCoordsToFontViewSpace(float x, float y)
  {
    auto ndc = ConvertToNDC(x, y);
    auto clipCoords = ConvertNDCToClipSpace(ndc.x, ndc.y);
    auto worldCoords = ConvertClipToFontViewSpace(clipCoords);
    return worldCoords;
  }
}
