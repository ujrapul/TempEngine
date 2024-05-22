// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Logger.hpp"
#include "String.hpp"
#include <iostream>

namespace Temp::Logger
{
  void Cout::Log(const char *text) const
  {
    std::cout << text << "\n";
  }

  void Cout::Log(const unsigned char *text) const
  {
    std::cout << text << "\n";
  }

  void Cout::Log(const BaseString<MemoryManager::Data::TEMP> &text) const
  {
    Log(text.c_str());
  }

  void Cout::LogErr(const char *text) const
  {
    std::cout << "[  \033[91m\033[1m" << text << "\033[0m  ]\n";
  }

  void Cout::LogErr(const unsigned char *text) const
  {
    std::cout << "[  \033[91m\033[1m" << text << "\033[0m  ]\n";
  }

  void Cout::LogErr(const BaseString<MemoryManager::Data::TEMP> &text) const
  {
    LogErr(text.c_str());
  }
}
