// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::Component::Updateable
{
  struct Data
  {
    void *data{nullptr};
    int type{};

    constexpr bool operator==(const Data&) const = default;
  };

  void Update(Scene::Data& scene, Data& updateable, float deltaTime);
}