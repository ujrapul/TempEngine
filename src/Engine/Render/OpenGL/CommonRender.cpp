// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "CommonRender.hpp"
#include "Engine.hpp"
#include "Editor.hpp"
#ifdef EDITOR
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#ifdef __linux__
#include "imgui_impl_x11.hpp"
#elif _WIN32
#include "imgui_impl_win32.h"
#endif
#endif

namespace Temp::Render
{
#ifdef EDITOR
  void RenderImGui(Scene::Data& scene, Event::Data& EventData)
  {
    ImGui_ImplOpenGL3_NewFrame();
#ifdef __linux__
    ImGui_ImplX11_NewFrame();
#elif _WIN32
    ImGui_ImplWin32_NewFrame();
#endif
    ImGui::NewFrame();

    Editor::RunImGui(scene, EventData);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
#endif
}
