// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Hoverable.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "Shader.hpp"
#ifdef EDITOR
#include "Drawable.hpp"
#endif

namespace Temp::Component::Hoverable
{
#ifdef EDITOR
  void ConstructDrawable(Hoverable::Data& hoverable)
  {
    float offsetX = hoverable.offset.x * hoverable.scale.x;
    float offsetY = hoverable.offset.y * hoverable.scale.y;
    float beginX = hoverable.x + offsetX;
    float beginY = hoverable.y + offsetY;
    float endX = beginX + hoverable.width * hoverable.scale.x;
    float endY = beginY + hoverable.height * hoverable.scale.y;

    // clang-format off
    float vertices[8] = {
      // positions      
      endX, endY,   // top right
      endX, beginY,  // bottom right
      beginX, beginY, // bottom left
      beginX, endY,   // top left
    };
    GLuint indices[6] = {
      0, 1, 3, // First Triangle
      1, 2, 3  // Second Triangle
    };
    // clang-format on

    hoverable.drawable.entity = Entity::MAX - 1;
    hoverable.drawable.vertices.Replace(vertices, 8);
    hoverable.drawable.indices.Replace(indices, 6);
    Drawable::UpdateData(hoverable.drawable);
    Drawable::Construct(hoverable.drawable,
                        Render::EditorShaderIdx::HOVERABLE,
                        GL_STATIC_DRAW,
                        {2},
                        2);
    Drawable::SetTranslate(hoverable.drawable, {0});
  }

  void DestructDrawable(Hoverable::Data& hoverable) { Drawable::Destruct(hoverable.drawable); }
  void UpdateDrawable(Hoverable::Data& hoverable)
  {
    DestructDrawable(hoverable);
    ConstructDrawable(hoverable);
  }
#endif

  bool IsInside(const Data& hoverable, float x, float y)
  {
    auto viewSpaceCoords = Camera::ConvertScreenCoordsToViewSpace(x, y);
    float offsetX = hoverable.offset.x * hoverable.scale.x;
    float offsetY = hoverable.offset.y * hoverable.scale.y;
    float beginX = hoverable.x + offsetX;
    float beginY = hoverable.y + offsetY;
    float endX = beginX + hoverable.width * hoverable.scale.x;
    float endY = beginY + hoverable.height * hoverable.scale.y;

    return viewSpaceCoords.x >= beginX && viewSpaceCoords.x <= endX &&
           viewSpaceCoords.y >= beginY && viewSpaceCoords.y <= endY;
  }

  bool IsInsideRaycast(const Data& hoverable, float x, float y)
  {
    auto viewSpaceCoords = Camera::ConvertScreenCoordsToViewSpace(x, y);
    auto localSpaceCoords = hoverable.model.inverse() * viewSpaceCoords;
    auto rayOrigin = Math::Vec3f(localSpaceCoords.x, localSpaceCoords.y, 10.f);
    auto rayDirection = (Math::Vec3f(localSpaceCoords.x, localSpaceCoords.y, -10.f) - rayOrigin)
                          .normalize();

    Math::Vec3f intersectionPoint;
    return Math::RayMeshIntersect(rayOrigin, rayDirection, hoverable.triangles, intersectionPoint);
  }

  void HoverableEnter(Scene::Data& scene, Data& hoverable)
  {
    if (!hoverable.lastInside)
    {
      hoverable.HoverEnter(scene, hoverable);
      hoverable.lastInside = true;
    }
  }

  void HoverableLeave(Scene::Data& scene, Data& hoverable)
  {
    if (hoverable.lastInside)
    {
      hoverable.HoverLeave(scene, hoverable);
      hoverable.lastInside = false;
    }
  }

  // Drag should work for both FontView Entities and regular View Entities
  // Editor dragging will drag both at the same time
  // In game dragging doesn't exist... for now
  void Drag(Scene::Data& scene, Data& hoverable, float lastX, float lastY, float currX, float currY)
  {
    if (!hoverable.isDrag)
    {
      return;
    }

    auto currViewSpaceCoords = Camera::ConvertScreenCoordsToViewSpace(currX, currY);
    auto lastViewSpaceCoords = Camera::ConvertScreenCoordsToViewSpace(lastX, lastY);
    hoverable.x += (currViewSpaceCoords.x - lastViewSpaceCoords.x);
    hoverable.y += (currViewSpaceCoords.y - lastViewSpaceCoords.y);
    hoverable.Drag(scene, hoverable, hoverable.x, hoverable.y);
  }
};
