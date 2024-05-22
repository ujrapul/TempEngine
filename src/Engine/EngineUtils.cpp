// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "EngineUtils.hpp"
#include "Logger.hpp"
#ifdef __linux__
#include <dlfcn.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace Temp
{
  namespace
  {
#ifdef __linux__
    inline std::filesystem::path GetApplicationDirectory()
    {
      char buffer[PATH_MAX];
      ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
      if (len == -1)
      {
        std::cout << "FAILED TO READ EXEC DIR!";
        return "";
      }

      std::filesystem::path executablePath(buffer);
      std::filesystem::path path = executablePath.parent_path();

      return path;
    }
#elif __APPLE__
    inline std::filesystem::path GetApplicationDirectory()
    {
      char executablePath[PATH_MAX];
      uint32_t size = sizeof(executablePath);
      if (_NSGetExecutablePath(executablePath, &size) == 0)
      {
        std::filesystem::path executableDir(executablePath);
        return executableDir.parent_path();
      }
      return std::filesystem::path(); // Return an empty path if failed to retrieve the directory
    }
#elif _WIN32
    inline std::filesystem::path GetApplicationDirectory()
    {
      char executablePath[MAX_PATH];
      DWORD size = GetModuleFileNameA(NULL, executablePath, MAX_PATH);
      if (size != 0)
      {
        std::filesystem::path executableDir(executablePath);
        return executableDir.parent_path();
      }
      return std::filesystem::path(); // Return an empty path if failed to retrieve the directory
    }
#endif
  }

  const std::filesystem::path& ApplicationDirectory()
  {
    static std::filesystem::path applicationDirectory{GetApplicationDirectory()};
    return applicationDirectory;
  }

  const std::filesystem::path& AssetsDirectory()
  {
    static std::filesystem::path assetsDirectory{GetApplicationDirectory() / "Assets"};
    return assetsDirectory;
  }

  void* OpenDynamicLibrary(const std::string& name)
  {
    void* libHandle = nullptr;
#ifdef __linux__
    libHandle = dlopen(("lib" + name + ".so").c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!libHandle)
    {
      Logger::LogErr(String("[EngineUtils] Error loading library ") + dlerror());
    }
#endif
    return libHandle;
  }

  void* GetDynamicLibraryFn(void* libraryHandle, const std::string& fn)
  {
#ifdef __linux__
    return dlsym(libraryHandle, fn.c_str());
#endif
  }

  void CloseDynamicLibrary(void* libraryHandle)
  {
#ifdef __linux__
    if (dlclose(libraryHandle))
    {
      Logger::LogErr(String("[EngineUtils] Error closing library ") + dlerror());
    }
#endif
  }
}
