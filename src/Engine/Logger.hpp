// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

// #include "STDPCH.hpp"
#include "MemoryManager.hpp"

namespace Temp
{
  template<MemoryManager::Data::Type> struct BaseString;
}

namespace Temp::Logger
{
  enum class LogType
  {
    COUT,
    NOOP
  };

  inline LogType logType{LogType::COUT};

  struct LogInterface
  {
    virtual void Log(const char *text) const = 0;
    virtual void Log(const unsigned char *text) const = 0;
    virtual void Log(const BaseString<MemoryManager::Data::TEMP> &) const = 0;

    virtual void LogErr(const char *text) const = 0;
    virtual void LogErr(const unsigned char *text) const = 0;
    virtual void LogErr(const BaseString<MemoryManager::Data::TEMP> &) const = 0;
  };

  struct Noop : LogInterface
  {
    inline void Log(const char *) const override {}
    inline void Log(const unsigned char *) const override {}
    inline void Log(const BaseString<MemoryManager::Data::TEMP> &) const override {}

    inline void LogErr(const char *) const override {}
    inline void LogErr(const unsigned char *) const override {}
    inline void LogErr(const BaseString<MemoryManager::Data::TEMP> &) const override {}
  };

  struct Cout : LogInterface
  {
    void Log(const char *text) const override;
    void Log(const unsigned char *text) const override;
    void Log(const BaseString<MemoryManager::Data::TEMP> &text) const override;
    void LogErr(const char *text) const override;
    void LogErr(const unsigned char *text) const override;
    void LogErr(const BaseString<MemoryManager::Data::TEMP> &text) const override;
  };

  inline const LogInterface &GetLogger()
  {
    static const Cout cout{};
    static const Noop noop{};
    switch (logType)
    {
    case LogType::COUT:
      return cout;
    default:
      return noop;
    };
  }

  inline void Log(const char *text)
  {
    GetLogger().Log(text);
  }

  inline void Log(const unsigned char *text)
  {
    GetLogger().Log(text);
  }

  inline void Log(const BaseString<MemoryManager::Data::TEMP> &text)
  {
    GetLogger().Log(text);
  }

  inline void LogErr(const char *text)
  {
    GetLogger().LogErr(text);
  }

  inline void LogErr(const unsigned char *text)
  {
    GetLogger().LogErr(text);
  }

  inline void LogErr(const BaseString<MemoryManager::Data::TEMP> &text)
  {
    GetLogger().LogErr(text);
  }
}
