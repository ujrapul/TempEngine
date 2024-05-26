// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "EngineUtils.hpp"
#include "FileSystem.hpp"
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
    inline Path GetApplicationDirectory()
    {
      char buffer[PATH_MAX];
      ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
      if (len == -1)
      {
        Logger::LogErr("FAILED TO READ EXEC DIR!");
        return "";
      }

      Path executablePath(buffer);
      Path path = executablePath.ParentPath();

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

  Path ApplicationDirectory()
  {
    return Path(ApplicationDirectoryGlobal().c_str());
  }

  Path AssetsDirectory()
  {
    return Path(AssetsDirectoryGlobal().c_str());
  }

  const GlobalPath& ApplicationDirectoryGlobal()
  {
    static GlobalPath applicationDirectory(GetApplicationDirectory().c_str());
    return applicationDirectory;
  }

  const GlobalPath& AssetsDirectoryGlobal()
  {
    static GlobalPath assetsDirectory((GetApplicationDirectory() / "Assets").c_str());
    return assetsDirectory;
  }

  void* OpenDynamicLibrary(const char* name)
  {
    void* libHandle = nullptr;
#ifdef __linux__
    libHandle = dlopen((String("lib") + name + String(".so")).c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!libHandle)
    {
      Logger::LogErr(String("[EngineUtils] Error loading library ") + dlerror());
    }
#endif
    return libHandle;
  }

  void* GetDynamicLibraryFn(void* libraryHandle, const char* fn)
  {
#ifdef __linux__
    return dlsym(libraryHandle, fn);
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
