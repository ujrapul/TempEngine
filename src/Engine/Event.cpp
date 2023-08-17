// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Event.hpp"
#include "Engine.hpp"
#include "FontLoader.hpp"
#include "Hoverable.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
// #include "RenderUtils.hpp"
#ifdef EDITOR
#include "imgui.h"
#endif

namespace Temp::Event
{
  namespace
  {
#ifdef EDITOR
    SceneObject::Data* copiedObject{nullptr};
#endif
  }

#ifdef EDITOR
  void KeyPressed(Event::Data& EventData, Input::KeyboardCode key)
#else
  void KeyPressed(Event::Data&, Input::KeyboardCode)
#endif
  {
#ifdef EDITOR
    auto& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || io.WantTextInput)
    {
      return;
    }

    if (key == Input::KeyboardCode::KB_LeftCtrl || key == Input::KeyboardCode::KB_RightCtrl)
    {
      EventData.ctrlPressed = true;
    }
    else if (EventData.ctrlPressed && key == Input::KeyboardCode::KB_C && EventData.selectedObject)
    {
      copiedObject = EventData.selectedObject;
    }
    else if (EventData.ctrlPressed && key == Input::KeyboardCode::KB_V && copiedObject)
    {
      Global::SpawnObject(SceneObject::Copy(*copiedObject));
    }
#endif
  }

#ifdef EDITOR
  void KeyReleased(Event::Data& EventData, Input::KeyboardCode key)
#else
  void KeyReleased(Event::Data&, Input::KeyboardCode)
#endif
  {
#ifdef EDITOR
    auto& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard || io.WantTextInput)
    {
      return;
    }

    switch (key)
    {
      case Input::KeyboardCode::KB_LeftCtrl:
      case Input::KeyboardCode::KB_RightCtrl:
      {
        EventData.ctrlPressed = false;
      }
      break;
      case Input::KeyboardCode::KB_Delete:
      {
        if (EventData.selectedObject)
        {
          Global::RemoveObject(*EventData.selectedObject);
          EventData.selectedObject = nullptr;
        }
      }
      default:
        break;
    }
#endif
  }

  void Hover(Scene::Data& scene, Event::Data& EventData, float mouseX, float mouseY)
  {
#ifdef EDITOR
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
      return;
    }
#endif

    if (scene.state == Scene::State::RUN)
    {
      auto& hoverableArray = Scene::GetComponentArray<Component::Type::HOVERABLE>(scene);
      for (size_t i = 0; i < hoverableArray.size; ++i)
      {
        auto& hoverable = hoverableArray.array[i];
        if (Component::Hoverable::IsInsideRaycast(hoverable, mouseX, mouseY) ||
            Component::Hoverable::IsInside(hoverable, mouseX, mouseY))
        {
          Component::Hoverable::HoverableEnter(scene, hoverable);
        }
        else
        {
          Component::Hoverable::HoverableLeave(scene, hoverable);
        }
      }

      if (EventData.buttonPressed[1])
      {
        if (EventData.draggable)
        {
          auto& hoverable = *EventData.draggable;
          Component::Hoverable::Drag(scene,
                                     hoverable,
                                     (float)EventData.lastMouseX,
                                     (float)EventData.lastMouseY,
                                     mouseX,
                                     mouseY);
        }
#ifdef EDITOR
        else
        {
          Camera::Drag((float)EventData.lastMouseX,
                       (float)EventData.lastMouseY,
                       (float)mouseX,
                       (float)mouseY);
        }
#endif
      }
    }
    EventData.lastMouseX = (int)mouseX;
    EventData.lastMouseY = (int)mouseY;
  }

  void ButtonPressed(Scene::Data& scene,
                     Event::Data& EventData,
                     int button,
                     float mouseX,
                     float mouseY)
  {
    // Used to prevent any input widgets in the imgui windows from having focus
    // which otherwise could overwrite properties of objects on selection
#ifdef EDITOR
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
      return;
    }
    ImGui::SetWindowFocus(nullptr);
#endif

    if (button > (int)EventData.buttonPressed.size())
    {
      return;
    }

    EventData.buttonPressed[button] = true;
    if (scene.state == Scene::State::RUN && EventData.buttonPressed[1])
    {
      auto& hoverableArray = Scene::GetComponentArray<Component::Type::HOVERABLE>(scene);
      for (size_t i = 0; i < hoverableArray.size; ++i)
      {
        auto& hoverable = hoverableArray.array[i];
        Entity::id entity = hoverableArray.sparseEntities[i];
        if (Component::Hoverable::IsInsideRaycast(hoverable, mouseX, mouseY) ||
            Component::Hoverable::IsInside(hoverable, mouseX, mouseY))
        {
          EventData.selectedObject = &Scene::GetObject(scene, entity);
          EventData.draggable = &hoverable;
          break;
        }
      }
    }
  }

  void ButtonReleased(Scene::Data& scene,
                      Event::Data& EventData,
                      float mouseX,
                      float mouseY,
                      int button)
  {
#ifdef EDITOR
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
      return;
    }
#endif

    EventData.buttonPressed[button] = false;
    if (!EventData.buttonPressed[1])
    {
      EventData.draggable = nullptr;
    }

    if (button != 1)
    {
      return;
    }

    if (scene.state == Scene::State::RUN)
    {
      auto& hoverableArray = Scene::GetComponentArray<Component::Type::HOVERABLE>(scene);
      for (size_t i = 0; i < hoverableArray.size; ++i)
      {
        auto& hoverable = hoverableArray.array[i];
        if (Component::Hoverable::IsInsideRaycast(hoverable, mouseX, mouseY) ||
            Component::Hoverable::IsInside(hoverable, mouseX, mouseY))
        {
          hoverable.Click(scene, hoverable);
          break;
        }
      }
    }
  }

#ifndef __APPLE__
  void Resize(Event::Data& EventData)
#else
  void Resize(Event::Data&)
#endif
  {
#ifndef __APPLE__
    glViewport(0, 0, EventData.windowWidth, EventData.windowHeight);
#endif
  }

#ifndef __APPLE__
  void Resize(Scene::Data&, Event::Data& EventData, void*)
#else
  void Resize(Scene::Data&, Event::Data&, void*)
#endif
  {
#ifndef __APPLE__
    Resize(EventData);
#endif
  }

  void RenderSetup(Event::Data& EventData)
  {
    Logger::Log(glGetString(GL_VERSION));

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Temp::Font::LoadFont();
    Temp::Render::OpenGLWrapper::LoadShaders();

    Resize(EventData);

    EventData.renderBegin = clock();
  }

  void RenderRun(Scene::Data& scene, Event::Data& EventData)
  {
    // TODO: Use High Resolution Timer here
    clock_t end{clock()};
    {
      if (EventData.limitFps &&
          ((float)(end - EventData.renderBegin) / CLOCKS_PER_SEC) < EventData.fps60)
      {
        return;
      }
      EventData.renderBegin = clock();
    }

    static const auto& bgColor = Global::GetBackgroundColor();
    glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Scene::Draw(scene);

    // NOTE: Testing Code
    // static float time = 0;
    // static Render::Square square;
    // Render::Initialize(square);
    // Render::Draw(square, time);
    // Render::Clean(square);
    // time += Global::DeltaTime();
  }

  void RenderClean(Data&)
  {
    // TODO: Clean up resources
  }
}
