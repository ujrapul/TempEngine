// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "EditorLevel.hpp"

namespace Temp::Scene::EditorLevel
{
  namespace
  {
    // void Construct(Scene::Data& /*unused*/)
    // {
    // }

    // void Update(Scene::Data& /*unused*/, float /*unused*/)
    // {
    // }

    // void DrawConstruct(Scene::Data& /*unused*/)
    // {
    // }

    // void DrawUpdate(Scene::Data& scene)
    // {
    //   Temp::Camera::UpdateFontOrthoScale(scene, (1080.f / Temp::Camera::GetHeight()));
    // }

//    void Destruct(Scene::Data& /*unused*/)
//    {
//    }

    // void DrawDestruct(Scene::Data& /*unused*/)
    // {
    // }
  }

  SceneFns Create()
  {
    SceneFns scene;
    // scene.ConstructFunc = Scene::Construct;
    // scene.Update = Scene::Update;
    // scene.DrawConstructFunc = Scene::DrawConstruct;
    // scene.DrawUpdateFunc = DrawUpdate;
    // scene.DestructFunc = Scene::Destruct;
    // scene.DrawDestructFunc = Scene::DrawDestruct;
    return scene;
  }
}
