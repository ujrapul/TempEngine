// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "CommonRender.hpp"
#include "Engine.hpp"
#include "Editor.hpp"
#include "Event.hpp"
#include "MemoryManager.hpp"
#if defined(EDITOR) || defined(DEBUG)
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
#if defined(EDITOR) || defined(DEBUG)
  namespace
  {
    void CommonRunImGui(Event::Data& EventData)
    {
      // Add your ImGui UI code here
      // bool show_demo_window;
      // ImGui::ShowDemoWindow(&show_demo_window);

      ImGui::SetNextWindowSize({250, 250});
      ImGui::SetNextWindowPos({EventData.windowWidth - 250.f, 0});
      ImGui::Begin("Statistics",
                   NULL,
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoBackground);
      ImGui::Text("%s",
                  (String("Global Memory:      ") +
                   String::ToString(MemoryManager::data.globalArena.offset / 1000000.f))
                    .c_str());
      ImGui::Text("%s",
                  (String("Scene Memory:       ") +
                   String::ToString(MemoryManager::data.sceneArena.offset / 1000000.f))
                    .c_str());
      ImGui::Text("%s",
                  (String("Temp Memory:        ") +
                   String::ToString(MemoryManager::data.tempArena.offset / 1000000.f))
                    .c_str());
      ImGui::Text("%s",
                  (String("Temp Thread Memory: ") +
                   String::ToString(MemoryManager::data.threadTempArena.offset / 1000000.f))
                    .c_str());
      ImGui::End();
    }
  }

  void RenderImGui(Scene::Data& scene, Event::Data& EventData)
  {
    ImGui_ImplOpenGL3_NewFrame();
#ifdef __linux__
    ImGui_ImplX11_NewFrame();
#elif _WIN32
    ImGui_ImplWin32_NewFrame();
#endif
    ImGui::NewFrame();

#ifdef EDITOR
    Editor::RunImGui(scene, EventData);
#endif
#ifdef DEBUG
    CommonRunImGui(EventData);
#endif

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
#endif
}

#if defined(EDITOR) || defined(DEBUG)
template void IM_DELETE<ImGuiContext>(ImGuiContext*);
template void IM_DELETE<ImFontAtlas>(ImFontAtlas*);
template void IM_DELETE<char>(char*);
template void IM_DELETE<ImGuiWindow>(ImGuiWindow*);
template void IM_DELETE<ImGuiViewportP>(ImGuiViewportP*);
template void IM_DELETE<ImFont>(ImFont*);
#endif
