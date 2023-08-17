// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Hoverable.hpp"
#include "Drawable.hpp"
#include "Scene.hpp"

namespace Temp
{
  template <typename T>
  void SetHoverableCallbacks(
      Scene::Data &scene,
      T &data,
      void (*Click)(Scene::Data &, Component::Hoverable::Data &) = [](auto &, auto &) {},
      void (*HoverEnter)(Scene::Data &, Component::Hoverable::Data &) = [](auto &, auto &) {},
      void (*HoverLeave)(Scene::Data &, Component::Hoverable::Data &) = [](auto &, auto &) {})
  {
    auto &hoverable = Scene::Get<Component::Type::HOVERABLE>(scene, data.entity);
    hoverable.Click = Click;
    hoverable.HoverEnter = HoverEnter;
    hoverable.HoverLeave = HoverLeave;
  }

  template <typename T>
  void SetHoverableCallbacks(
      Scene::Data &scene,
      T &data,
      void *callbackData,
      void (*Click)(Scene::Data &, Component::Hoverable::Data &) = [](auto &, auto &) {},
      void (*HoverEnter)(Scene::Data &, Component::Hoverable::Data &) = [](auto &, auto &) {},
      void (*HoverLeave)(Scene::Data &, Component::Hoverable::Data &) = [](auto &, auto &) {})
  {
    auto &hoverable = Scene::Get<Component::Type::HOVERABLE>(scene, data.entity);
    hoverable.Click = Click;
    hoverable.HoverEnter = HoverEnter;
    hoverable.HoverLeave = HoverLeave;
    hoverable.callbackData = callbackData;
  }

  template <typename T>
  void Drag(Scene::Data &scene, Component::Hoverable::Data &hoverable, float x, float y)
  {
    auto &data = *static_cast<T*>(hoverable.callbackData);
    auto &drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, data.entity);
    // auto &scale = Scene::Get<Temp::Component::Type::SCALE>(scene, data.entity);
    Component::Drawable::SetTranslate(drawable, {x, y, 0});
  }
}