// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Math.hpp"
#ifdef EDITOR
#include "Drawable.hpp"
#endif
#include <cfloat>

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Component::Hoverable
{
  constexpr void NoOp(Scene::Data&, struct Data&) {}
  constexpr void NoOpDrag(Scene::Data&, struct Data&, float, float) {}

  struct Data
  {
    void (*Click)(Scene::Data&, Data&){NoOp};
    void (*HoverEnter)(Scene::Data&, Data&){NoOp};
    void (*HoverLeave)(Scene::Data&, Data&){NoOp};
    void (*Drag)(Scene::Data&, Data&, float, float){NoOpDrag};
    void* callbackData{nullptr};
    float x{FLT_MIN};
    float y{FLT_MIN};
    float width{0};
    float height{0};
    Math::Vec2f scale{1.f, 1.f};
    bool lastInside{false};
    bool isDrag{false};
    Math::Vec2f offset{};
    Math::Mat4 model{};
    std::vector<std::vector<Math::Vec3f>> triangles{};
#ifdef EDITOR
    Drawable::Data drawable{};
#endif

    bool operator==(const Data& other) const = default;
  };

  inline std::ostream& operator<<(std::ostream& os, const std::vector<Math::Vec3f>& triangle)
  {
    os << "{";
    for (const auto& vertex : triangle)
    {
      os << vertex << ",";
    }
    os << "}\n";
    return os;
  }

  inline std::ostream& operator<<(std::ostream& os,
                                  const std::vector<std::vector<Math::Vec3f>>& triangles)
  {
    os << "Triangles{";
    for (const auto& triangle : triangles)
    {
      os << triangle << ",";
    }
    os << "}\n";
    return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const Data& hoverable)
  {
    os << "Hoverable(" << hoverable.Click << " " << hoverable.HoverEnter << " "
       << hoverable.HoverLeave << " " << hoverable.Drag << " " << hoverable.callbackData << " "
       << hoverable.x << " " << hoverable.y << " " << hoverable.width << " " << hoverable.y << " "
       << hoverable.width << " " << hoverable.height << " " << hoverable.scale << " "
       << hoverable.lastInside << " " << hoverable.isDrag << " " << hoverable.model << " "
       << hoverable.triangles << " "
       << ")\n";
    return os;
  }

#ifdef EDITOR
  void ConstructDrawable(Hoverable::Data& hoverable);
  void DestructDrawable(Hoverable::Data& hoverable);
  void UpdateDrawable(Hoverable::Data& hoverable);
#endif

  // Only works for Rectangles
  // Add more interior detections as needed
  bool IsInside(const Data& hoverable, float x, float y);
  bool IsInsideRaycast(const Data& hoverable, float x, float y);
  void HoverableEnter(Scene::Data& scene, Data& hoverable);
  void HoverableLeave(Scene::Data& scene, Data& hoverable);

  // Drag should work for both FontView Entities and regular View Entities
  // Editor dragging will drag both at the same time
  // In game dragging doesn't exist... for now
  void Drag(Scene::Data& scene,
            Data& hoverable,
            float lastX,
            float lastY,
            float currX,
            float currY);
};
