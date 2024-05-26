// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "TextButton.hpp"
#include "FontLoader.hpp"
#include "Hoverable.hpp"
#include "Scene.hpp"
#include "TextBox.hpp"

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::TextButton
{
  namespace
  {
#ifdef EDITOR
    void Drag(Scene::Data& scene, Component::Hoverable::Data& hoverable, float x, float y)
    {
      auto& textButton = *static_cast<TextButton::Data*>(hoverable.callbackData);
      auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene,
                                                                   textButton.textBox.entity);
      auto& position = Scene::Get<Temp::Component::Type::POSITION2D>(scene,
                                                                     textButton.textBox.entity);
      position.x = x;
      position.y = y;
      Component::Drawable::SetTranslate(drawable, {x, y, 0});
    }
#endif
  }

  void Construct(Scene::Data& scene, Data& textButton, ConstructData& ctorData, Entity::id entity)
  {
    textButton.entity = entity;
    textButton.textBox.hasParent = true;
    ctorData.hoverable.callbackData = &textButton;
    ctorData.textBoxCtorData.offsetY = Font::Padding() * ctorData.textBoxCtorData.scale * 1.5f;
#ifdef EDITOR
    ctorData.hoverable.isDrag = true;
    ctorData.hoverable.Drag = Drag;
#endif
    Scene::AddComponent<Component::Type::HOVERABLE>(scene, textButton.entity, ctorData.hoverable);

    TextBox::Construct(scene, textButton.textBox, ctorData.textBoxCtorData, entity);
    SetHoverableCallbacks(scene, textButton);
  }

  void DrawConstruct(Scene::Data& scene, Data& textButton, int shaderType)
  {
    TextBox::DrawConstruct(scene, textButton.textBox, shaderType);
#ifdef EDITOR
    auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textButton.entity);
    Component::Hoverable::ConstructDrawable(hoverable);
#endif
  }

  void DrawUpdate(Scene::Data& scene, Component::Drawable::Data& drawable, Data& textButton)
  {
    TextBox::DrawUpdate(scene, drawable, textButton.textBox);
#ifdef EDITOR
    auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textButton.entity);
    Component::Hoverable::UpdateDrawable(hoverable);
#endif
  }

  void UpdateText(Scene::Data& scene, Data& textButton, const char* newText)
  {
    TextBox::UpdateText(scene, textButton.textBox, newText);
#ifdef EDITOR
    auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textButton.entity);
    Component::Hoverable::UpdateDrawable(hoverable);
#endif
  }

  void DrawDestruct(Scene::Data& scene, Data& textButton)
  {
    TextBox::DrawDestruct(scene, textButton.textBox);
#ifdef EDITOR
    auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textButton.entity);
    Component::Hoverable::DestructDrawable(hoverable);
#endif
  }

  void Destruct(Scene::Data& scene, Data& textButton)
  {
    TextBox::Destruct(scene, textButton.textBox);
  }

  void HoverEnter(Temp::Scene::Data& scene, Temp::Component::Hoverable::Data& hoverable)
  {
    auto* button = static_cast<TextButton::Data*>(hoverable.callbackData);
    TextBox::EnableOutline(scene, button->textBox, true);
  }

  void HoverLeave(Temp::Scene::Data& scene, Temp::Component::Hoverable::Data& hoverable)
  {
    auto* button = static_cast<TextButton::Data*>(hoverable.callbackData);
    TextBox::EnableOutline(scene, button->textBox, false);
  }

  void SetHoverableCallbacks(Scene::Data& scene,
                             Data& textButton,
                             void (*Click)(Scene::Data&, Component::Hoverable::Data&),
                             void (*HoverEnter)(Scene::Data&, Component::Hoverable::Data&),
                             void (*HoverLeave)(Scene::Data&, Component::Hoverable::Data&))
  {
    auto& hoverable = Scene::Get<Component::Type::HOVERABLE>(scene, textButton.entity);
    hoverable.Click = Click;
    hoverable.HoverEnter = HoverEnter;
    hoverable.HoverLeave = HoverLeave;
  }

  void SetHoverableCallbacks(Scene::Data& scene,
                             Data& textButton,
                             void* callbackData,
                             void (*Click)(Scene::Data&, Component::Hoverable::Data&),
                             void (*HoverEnter)(Scene::Data&, Component::Hoverable::Data&),
                             void (*HoverLeave)(Scene::Data&, Component::Hoverable::Data&))
  {
    auto& hoverable = Scene::Get<Component::Type::HOVERABLE>(scene, textButton.entity);
    hoverable.Click = Click;
    hoverable.HoverEnter = HoverEnter;
    hoverable.HoverLeave = HoverLeave;
    textButton.callbackData = callbackData;
  }

  void Scale(Scene::Data& scene, Data& textButton, float scale)
  {
    auto& drawable = Scene::Get<Component::Type::DRAWABLE>(scene, textButton.entity);
    drawable.offset = {0, Font::Padding() * scale * 1.5f, 0};
    SceneObject::Scale(scene, Scene::GetObject(scene, textButton.entity), scale);
  }

  void SetVisiblity(Scene::Data& scene, Data& textButton, bool visible)
  {
    auto& drawable = Scene::Get<Component::Type::DRAWABLE>(scene, textButton.entity);
    drawable.visible = visible;
  }
}
