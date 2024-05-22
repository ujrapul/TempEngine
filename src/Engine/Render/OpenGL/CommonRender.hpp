// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#if defined(EDITOR) || defined(DEBUG)
#include "imgui.h"
#include "imgui_internal.h"
#ifdef __linux
#include "imgui_impl_opengl3.h"
#endif
#endif

namespace Temp::Event
{
  struct Data;
}

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Render
{
#if defined(EDITOR) || defined(DEBUG)
  void RenderImGui(Scene::Data& scene, Event::Data& EventData);
#endif
}

#if defined(EDITOR) || defined(DEBUG)
extern template void IM_DELETE<ImGuiContext>(ImGuiContext*);
extern template void IM_DELETE<ImFontAtlas>(ImFontAtlas*);
extern template void IM_DELETE<char>(char*);
extern template void IM_DELETE<ImGuiWindow>(ImGuiWindow*);
extern template void IM_DELETE<ImGuiViewportP>(ImGuiViewportP*);
extern template void IM_DELETE<ImFont>(ImFont*);
extern template IMGUI_API bool ImGui::CheckboxFlagsT<int>(char const*, int*, int);
extern template IMGUI_API bool ImGui::CheckboxFlagsT<unsigned int>(char const*, unsigned int*, unsigned int);
extern template IMGUI_API bool ImGui::CheckboxFlagsT<long long>(char const*, long long*, long long);
extern template IMGUI_API bool ImGui::CheckboxFlagsT<unsigned long long>(char const*, unsigned long long*, unsigned long long);
extern template IMGUI_API bool ImGui::DragBehaviorT<int, int, float>(int, int*, float, int, int, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<unsigned int, int, float>(int, unsigned int*, float, unsigned int, unsigned int, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<int, int, float>(int, int*, float, int, int, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<unsigned int, int, float>(int, unsigned int*, float, unsigned int, unsigned int, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<int, int, float>(int, int*, float, int, int, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<unsigned int, int, float>(int, unsigned int*, float, unsigned int, unsigned int, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<long long, long long, double>(int, long long*, float, long long, long long, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<unsigned long long, long long, double>(int, unsigned long long*, float, unsigned long long, unsigned long long, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<float, float, float>(int, float*, float, float, float, char const*, int);
extern template IMGUI_API bool ImGui::DragBehaviorT<double, double, double>(int, double*, float, double, double, char const*, int);
extern template IMGUI_API bool ImGui::SliderBehaviorT<int, int, float>(ImRect const&, unsigned int, int, int*, int, int, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<unsigned int, int, float>(ImRect const&, unsigned int, int, unsigned int*, unsigned int, unsigned int, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<int, int, float>(ImRect const&, unsigned int, int, int*, int, int, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<unsigned int, int, float>(ImRect const&, unsigned int, int, unsigned int*, unsigned int, unsigned int, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<int, int, float>(ImRect const&, unsigned int, int, int*, int, int, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<unsigned int, int, float>(ImRect const&, unsigned int, int, unsigned int*, unsigned int, unsigned int, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<long long, long long, double>(ImRect const&, unsigned int, int, long long*, long long, long long, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<unsigned long long, long long, double>(ImRect const&, unsigned int, int, unsigned long long*, unsigned long long, unsigned long long, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<float, float, float>(ImRect const&, unsigned int, int, float*, float, float, char const*, int, ImRect*);
extern template IMGUI_API bool ImGui::SliderBehaviorT<double, double, double>(ImRect const&, unsigned int, int, double*, double, double, char const*, int, ImRect*);
#endif
