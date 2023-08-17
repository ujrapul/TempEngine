// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Input.hpp"
#include <array>
#include <thread>

namespace Temp
{
  namespace Component::Hoverable
  {
    struct Data;
  }

  namespace SceneObject
  {
    struct Data;
  }

  namespace Scene
  {
    struct Data;
  }
}

namespace Temp::Event
{
  struct Data
  {
    clock_t renderBegin{clock()};

    Component::Hoverable::Data* draggable{nullptr};
    SceneObject::Data* selectedObject{nullptr};

    const float fps60{0.0166666f * 2.f};
    const float fps30{0.0333333f * 2.f};
    int windowWidth{};
    int windowHeight{};
    int lastMouseX{};
    int lastMouseY{};
    bool limitFps{false};
    bool isInFullScreen{false};
#ifdef EDITOR
    bool ctrlPressed{false};
#endif
    std::array<bool, 3> buttonPressed = {false, false, false};
  };

  void KeyPressed(Event::Data& EventData, Input::KeyboardCode key);
  void KeyReleased(Event::Data& EventData, Input::KeyboardCode key);
  void Hover(Scene::Data& scene, Event::Data& EventData, float mouseX, float mouseY);
  void ButtonPressed(Scene::Data& scene, Event::Data& EventData, int button, float mouseX, float mouseY);
  void ButtonReleased(Scene::Data& scene, Event::Data& EventData, float mouseX, float mouseY, int button);
  void Resize(Data& EventData);
  void Resize(Scene::Data& scene, Data& EventData, void* data);
  void RenderSetup(Data& EventData);
  void RenderRun(Scene::Data& scene, Data& EventData);
  void RenderClean(Data& EventData);
}
