// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "UT_ComponentContainer.hpp"
#include "UT_ComponentData.hpp"
#include "UT_Entity.hpp"
#include "UT_Event.hpp"
#include "UT_Hoverable.hpp"
#include "UT_LevelSerializer.hpp"
#include "UT_Math.hpp"
#include "UT_Scene.hpp"
#include "UT_ThreadPool.hpp"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

int main()
{
  using namespace Temp;

  auto timer = Timer("Unit Test Execution");

  Logger::logType = Logger::LogType::NOOP;
  Math::UnitTests::Run();
  Component::UnitTests::Run();
  Component::Container::UnitTests::Run();
  Component::Hoverable::UnitTests::Run();
  Event::UnitTests::Run();
  LevelSerializer::UnitTests::Run();
  Logger::logType = Logger::LogType::NOOP;
  ThreadPool::UnitTests::Run();
  Scene::UnitTests::Run();
  Entity::UnitTests::Run();

  Logger::logType = Logger::LogType::COUT;
  std::cout << "Unit Tests Passed!" << std::endl;

  return 0;
}
