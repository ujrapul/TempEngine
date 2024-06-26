# SPDX-FileCopyrightText: 2023 Ujwal Vujjini
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.25.0)
project(TEMP_ENGINE)

include(ProcessorCount)

#if (MSVC)
#  set(CMAKE_GENERATOR_PLATFORM "x64" CACHE STRING "Visual Studio 17 2022" FORCE)
#endif()

include(ExternalProject)

if (APPLE)
  set(CMAKE_MACOSX_RPATH 1)
endif()

set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
if (APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path")
else()
  set(CMAKE_INSTALL_RPATH "\${ORIGIN}")  # Use $ORIGIN to refer to the executable directory
endif()
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (MSVC)
  add_compile_definitions(
    $<$<CXX_COMPILER_ID:MSVC>:"$<$<CONFIG:DEBUG>:DEBUG>">
    $<$<CXX_COMPILER_ID:MSVC>:"$<$<CONFIG:RELEASE>:RELEASE>">
    $<$<CXX_COMPILER_ID:MSVC>:"$<$<CONFIG:RELWITHDEBINFO>:DRELEASE>">
    
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:"$<$<CONFIG:DEBUG>:-DDEBUG>">
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:"$<$<CONFIG:RELEASE>:-DRELEASE>">
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:"$<$<CONFIG:RELWITHDEBINFO>:-DRELEASE>">
  )
else()
  add_compile_definitions(
    "$<$<CONFIG:DEBUG>:-DDEBUG>"
    "$<$<CONFIG:RELEASE>:-DRELEASE>"
    "$<$<CONFIG:RELWITHDEBINFO>:-DRELEASE>"
  )
endif()

if (UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if (LINUX)
  execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
  if ("${ld_version}" MATCHES "GNU gold")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
  endif()
endif()

# Find the OpenGL package
find_package(OpenGL REQUIRED)
if (LINUX)
  find_package(X11 REQUIRED)
endif()

if (APPLE)
  enable_language(OBJCXX)
endif()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (MSVC)
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # set(CMAKE_UNITY_BUILD ON)
  if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
  else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
  endif()
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
  if (UNIX)
    # Needed for hiding GNU_UNIQUE_OBJECT symbols which were causing issues with unloading shared libraries
    # Perhaps there's a clash between the symbols in the executable and the ones in the shared library since
    # they both compile with the same engine source code.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fpic -Wl,-no-gnu-unique -O0")
    # Use -fno-implicit-templates to check templates being instantiated
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fpic -Wl,-no-gnu-unique -fno-implicit-templates -O0")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fpic -Wl,-no-gnu-unique -ftime-report -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -fpic")
    # set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")
  endif()
endif()

# Only for Linux and Mac
if (SANITIZE_THREAD STREQUAL "ON")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
elseif (SANITIZE_ADDRESS STREQUAL "ON")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_VERBOSE_BUILD ON)
if (USE_CLANG_TIDY STREQUAL "ON")
  set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -checks='-*,performance-*,modernize-*,-modernize-use-trailing-return-type,-modernize-avoid-c-arrays,-clang-analyzer-cplusplus.NewDeleteLeaks,';
    -fix;)
endif()

set(TARGET_ENGINE TempEngine)
set(TARGET_ENGINE_EDITOR TempEngineEditor)
set(TARGET_ENGINE_UNIT_TESTS UT_Engine)

file(GLOB_RECURSE SOURCE_LIST
  "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.glsl"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.h"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.level"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.lua"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.m"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.mm"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.txt"
  "${CMAKE_CURRENT_LIST_DIR}/src/*.cmake"
  "*.sh"
  "*.md"
)

function(target_include_directories_recursive target_name dir_list)
    # Process each directory in the list
    foreach(dir ${dir_list})
        # Add the current directory to the target
        target_include_directories(${target_name} PRIVATE ${dir})

        # Get the list of subdirectories in the current directory
        file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${dir} ${dir}/*)

        # Process subdirectories recursively
        foreach(subdir ${subdirs})
            if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${subdir})
                target_include_directories_recursive(${target_name} ${dir}/${subdir})
            endif()
        endforeach()
    endforeach()
endfunction()

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${SOURCE_LIST})
if (MSVC)
  add_library(Development STATIC ${SOURCE_LIST})
  target_compile_options(Development PRIVATE /std:c++20)
  target_include_directories_recursive(Development src)
  target_include_directories_recursive(Development "C:/libsndfile/include")
else()
  add_custom_target(Development SOURCES ${SOURCE_LIST})
endif()

# TODO: When you become more knowledgable try writing your own font loading system
add_subdirectory(${SRC_DIR}/Engine/Dependencies/freetype ${CMAKE_BINARY_DIR}/freetype)
set_target_properties(freetype PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:freetype,INTERFACE_INCLUDE_DIRECTORIES>)

set ( IMGUI_SRC
  ${SRC_DIR}/Engine/Dependencies/imgui/imgui.cpp
  ${SRC_DIR}/Engine/Dependencies/imgui/imgui_demo.cpp
  ${SRC_DIR}/Engine/Dependencies/imgui/imgui_draw.cpp
  ${SRC_DIR}/Engine/Dependencies/imgui/imgui_tables.cpp
  ${SRC_DIR}/Engine/Dependencies/imgui/imgui_widgets.cpp
  ${SRC_DIR}/Engine/Dependencies/imgui/backends/imgui_impl_opengl3.cpp
)

if (APPLE)
  list (APPEND IMGUI_SRC
    ${SRC_DIR}/Engine/Dependencies/imgui/backends/imgui_impl_osx.mm
    ${SRC_DIR}/Engine/Dependencies/imgui/backends/imgui_impl_opengl2.cpp
  )
elseif(WIN32)
  list (APPEND IMGUI_SRC
    ${SRC_DIR}/Engine/Dependencies/imgui/backends/imgui_impl_win32.cpp
  )
endif()

# Add the ImGui library
add_library(imgui
  OBJECT
  ${IMGUI_SRC}
)
set_target_properties(
  imgui
  PROPERTIES
    CXX_CLANG_TIDY ""
)
target_compile_definitions(imgui PUBLIC GL_SILENCE_DEPRECATION -DIMGUI_DISABLE_OBSOLETE_KEYIO)

set_target_properties(imgui PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:imgui,INTERFACE_INCLUDE_DIRECTORIES>)
target_include_directories(imgui
  PUBLIC ${SRC_DIR}/Engine/Dependencies/imgui
  PUBLIC ${SRC_DIR}/Engine/Dependencies/imgui/backends
)

set (STD_PCH_SRC
  ${CMAKE_CURRENT_LIST_DIR}/src/Engine/STDPCH.cpp
)
set (STD_PCH_HEADER
  ${CMAKE_CURRENT_LIST_DIR}/src/Engine/STDPCH.hpp
)

# Return to when import support with PCH headers and includes is fully supported
# # compile a list of system modules
# function (compile_system_header SYSTEM_FILE)
#   if (LINUX)
#     if (NOT EXISTS "${CMAKE_BINARY_DIR}/gcm.cache/usr/include/c++/13.2.1/${SYSTEM_FILE}.gcm")
#       execute_process(
#         COMMAND ${CMAKE_CXX_COMPILER} -fmodules-ts -std=c++20 -c -x c++-system-header ${SYSTEM_FILE}
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#       )
#     endif()
#   endif()
# endfunction()

# compile_system_header(iostream)
# compile_system_header(string)
# compile_system_header(vector)
# compile_system_header(thread)

add_subdirectory(src/Engine)
# add_subdirectory(src/Nunu)
add_subdirectory(src/NorthernQuarter)
add_subdirectory(src/UT_Engine)
