// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Entity.hpp"
#include "Math.hpp"
#include <cstdint>

namespace Temp::Scene
{
  struct Data;
}

namespace Temp::ParticleSystem
{
  enum class EmissionType : uint8_t
  {
    RADIAL_ROTATE,
    MAX,
  };

  struct Parameters
  {
    Math::Vec3f initialPos{};
    SceneString textureName{};
    float initialScale{};
    float time{};
    float speed{};
    float spawnRate{};
    float lifeTime{};
    int maxCount{};
    Entity::id entity{};
    EmissionType emissionType{EmissionType::RADIAL_ROTATE};
  };

  inline Parameters dummy;

  struct Data
  {
    std::vector<float> posX{};
    std::vector<float> posY{};
    std::vector<float> posZ{};

    std::vector<float> rotX{};
    std::vector<float> rotY{};
    std::vector<float> rotZ{};

    std::vector<float> scaleX{};
    std::vector<float> scaleY{};
    std::vector<float> scaleZ{};

    std::vector<float> lifeTime{};

    Math::Vec3f direction{0.1f};
    float spawnTime{};
    int maxIdx{};
    
    uint8_t posX_VBO{};
    uint8_t posY_VBO{};
    uint8_t posZ_VBO{};
    
    uint8_t rotX_VBO{};
    uint8_t rotY_VBO{};
    uint8_t rotZ_VBO{};
    
    uint8_t scaleX_VBO{};
    uint8_t scaleY_VBO{};
    uint8_t scaleZ_VBO{};

    Parameters& params{dummy};
  };

  struct SystemsData
  {
    std::vector<Data> particleSystems{};
  };

  void Init(Scene::Data& scene, SystemsData& systems, Parameters& params, int count);
  void Update(SystemsData& systems);
}