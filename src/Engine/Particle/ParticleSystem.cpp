// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "ParticleSystem.hpp"
#include "ComponentType.hpp"
#include "Drawable.hpp"
#include "Math_fwd.hpp"
#include "OpenGLWrapper.hpp"
#include "RenderUtils.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include <xmmintrin.h>

namespace Temp::ParticleSystem
{
  namespace
  {
    Render::Square square{};
  }

  void Init(Scene::Data& scene, SystemsData& systems, Parameters& params)
  {
    Data particleSystem;

    particleSystem.params = params;
    particleSystem.params.entity = Scene::CreateEntity(scene);

    particleSystem.posX.resize(particleSystem.params.maxCount);
    particleSystem.posY.resize(particleSystem.params.maxCount);
    particleSystem.posZ.resize(particleSystem.params.maxCount);

    particleSystem.rotX.resize(particleSystem.params.maxCount);
    particleSystem.rotY.resize(particleSystem.params.maxCount);
    particleSystem.rotZ.resize(particleSystem.params.maxCount);

    particleSystem.scaleX.resize(particleSystem.params.maxCount, 1.f);
    particleSystem.scaleY.resize(particleSystem.params.maxCount, 1.f);
    particleSystem.scaleZ.resize(particleSystem.params.maxCount, 1.f);

    particleSystem.lifeTime.resize(particleSystem.params.maxCount);

    systems.particleSystems.push_back(particleSystem);

    Component::Drawable::Data drawable;
    Component::Drawable::UpdateData(drawable,
                                    {std::begin(square.vertices), std::end(square.vertices)},
                                    {std::begin(square.indices), std::end(square.indices)});
    drawable.numInstances = particleSystem.params.maxCount;
    drawable.texture = Render::OpenGLWrapper::LoadTextureTGA(
      particleSystem.params.textureName.c_str(),
      GL_BGRA,
      GL_LINEAR);
    Component::Drawable::Construct(drawable, Render::ShaderIdx::PARTICLE);
    particleSystem.posX_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.posX,
      1,
      1,
      1,
      0);
    particleSystem.posY_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.posY,
      2,
      1,
      1,
      0);
    particleSystem.posZ_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.posZ,
      3,
      1,
      1,
      0);
    particleSystem.rotX_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.rotX,
      4,
      1,
      1,
      0);
    particleSystem.rotY_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.rotY,
      5,
      1,
      1,
      0);
    particleSystem.rotZ_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.rotZ,
      6,
      1,
      1,
      0);
    particleSystem.scaleX_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.scaleX,
      7,
      1,
      1,
      0);
    particleSystem.scaleY_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.scaleY,
      8,
      1,
      1,
      0);
    particleSystem.scaleZ_VBO = Temp::Component::Drawable::CreateFloatInstancedBuffer(
      drawable,
      particleSystem.scaleZ,
      9,
      1,
      1,
      0);
    Scene::AddComponent<Component::Type::DRAWABLE>(scene, particleSystem.params.entity, drawable);
  }

  void Update(SystemsData& systems, float deltaTime)
  {
    for (size_t i = 0; i < systems.particleSystems.size(); ++i)
    {
      auto& currParticleSystem = systems.particleSystems[i];
      const auto& currParams = currParticleSystem.params;
      if (currParticleSystem.spawnTime >= currParams.spawnRate)
      {
        int& maxIdx = currParticleSystem.maxIdx;
        if (currParticleSystem.maxIdx < currParams.maxCount)
        {
          maxIdx += 1;
        }
        currParticleSystem.spawnTime = 0;
      }
      else
      {
        currParticleSystem.spawnTime += deltaTime;
      }

      if (currParticleSystem.lifeTime.front() > currParams.lifeTime)
      {
        currParticleSystem.posX.erase(currParticleSystem.posX.begin());
        currParticleSystem.posY.erase(currParticleSystem.posY.begin());
        currParticleSystem.posZ.erase(currParticleSystem.posZ.begin());

        currParticleSystem.rotX.erase(currParticleSystem.rotX.begin());
        currParticleSystem.rotY.erase(currParticleSystem.rotY.begin());
        currParticleSystem.rotZ.erase(currParticleSystem.rotZ.begin());

        currParticleSystem.scaleX.erase(currParticleSystem.scaleX.begin());
        currParticleSystem.scaleY.erase(currParticleSystem.scaleY.begin());
        currParticleSystem.scaleZ.erase(currParticleSystem.scaleZ.begin());
      }

      switch (currParams.emissionType)
      {
        case EmissionType::RADIAL_ROTATE:
        {
          for (int j = 0; j < currParticleSystem.maxIdx; ++j)
          {
            currParticleSystem.posX[j] += currParticleSystem.direction.x * currParams.speed;
            currParticleSystem.posY[j] += currParticleSystem.direction.y * currParams.speed;
            currParticleSystem.posZ[j] += currParticleSystem.direction.z * currParams.speed;
            auto result = Math::Vec4f(currParticleSystem.direction) * Math::Mat4().rotateZ(5);
            currParticleSystem.direction.x = result.x;
            currParticleSystem.direction.y = result.y;
            currParticleSystem.direction.z = result.z;

            currParticleSystem.lifeTime[j] += deltaTime;
          }
        }
        break;
        case EmissionType::MAX:
          break;
      }

      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.posX_VBO,
                                       currParticleSystem.posX.data(),
                                       currParticleSystem.posX.size());
      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.posY_VBO,
                                       currParticleSystem.posY.data(),
                                       currParticleSystem.posY.size());
      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.posZ_VBO,
                                       currParticleSystem.posZ.data(),
                                       currParticleSystem.posZ.size());
      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.rotX_VBO,
                                       currParticleSystem.rotX.data(),
                                       currParticleSystem.rotX.size());
      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.rotY_VBO,
                                       currParticleSystem.rotY.data(),
                                       currParticleSystem.rotY.size());
      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.rotZ_VBO,
                                       currParticleSystem.rotZ.data(),
                                       currParticleSystem.rotZ.size());
      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.scaleX_VBO,
                                       currParticleSystem.scaleX.data(),
                                       currParticleSystem.scaleX.size());
      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.scaleY_VBO,
                                       currParticleSystem.scaleY.data(),
                                       currParticleSystem.scaleY.size());
      Render::OpenGLWrapper::UpdateVBO(currParticleSystem.scaleZ_VBO,
                                       currParticleSystem.scaleZ.data(),
                                       currParticleSystem.scaleZ.size());
    }
  }
}
