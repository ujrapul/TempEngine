# SPDX-FileCopyrightText: 2023 Ujwal Vujjini
# SPDX-License-Identifier: MIT

if (LINUX)
  # Audio libraries
  find_package(ALSA REQUIRED)
endif()

find_package(SndFile REQUIRED)

get_filename_component(CMAKE_SCRIPT_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)

set ( ENGINE_INCLUDE_DIRS
  ${CMAKE_SCRIPT_DIR}
  ${CMAKE_SCRIPT_DIR}/Audio
  ${CMAKE_SCRIPT_DIR}/Components
  ${CMAKE_SCRIPT_DIR}/Dependencies
  ${CMAKE_SCRIPT_DIR}/Dependencies/imgui
  ${CMAKE_SCRIPT_DIR}/Dependencies/imgui/backends
  ${CMAKE_SCRIPT_DIR}/Editor
  ${CMAKE_SCRIPT_DIR}/Entity
  ${CMAKE_SCRIPT_DIR}/LevelSerializer
  ${CMAKE_SCRIPT_DIR}/Memory
  ${CMAKE_SCRIPT_DIR}/Render
  ${CMAKE_SCRIPT_DIR}/Render/OpenGL
  ${CMAKE_SCRIPT_DIR}/Render/OpenGL/Shaders
  ${CMAKE_SCRIPT_DIR}/Render/glad
  ${CMAKE_SCRIPT_DIR}/Scene
)
set ( ENGINE_SRC
  ${CMAKE_SCRIPT_DIR}/Audio/AudioSystem.cpp
  ${CMAKE_SCRIPT_DIR}/Camera.cpp
  ${CMAKE_SCRIPT_DIR}/Components/ComponentContainer.cpp
  ${CMAKE_SCRIPT_DIR}/Components/Drawable.cpp
  ${CMAKE_SCRIPT_DIR}/Components/Hoverable.cpp
  ${CMAKE_SCRIPT_DIR}/Components/Updateable.cpp
  ${CMAKE_SCRIPT_DIR}/Engine.cpp
  ${CMAKE_SCRIPT_DIR}/EngineUtils.cpp
  ${CMAKE_SCRIPT_DIR}/Entity/EntityData.cpp
  ${CMAKE_SCRIPT_DIR}/Entity/Sprite.cpp
  ${CMAKE_SCRIPT_DIR}/Entity/TextBox.cpp
  ${CMAKE_SCRIPT_DIR}/Entity/TextButton.cpp
  ${CMAKE_SCRIPT_DIR}/Event.cpp
  ${CMAKE_SCRIPT_DIR}/FontLoader.cpp
  ${CMAKE_SCRIPT_DIR}/Input.cpp
  ${CMAKE_SCRIPT_DIR}/LevelSerializer/LevelSerializer.cpp
  ${CMAKE_SCRIPT_DIR}/Logger.cpp
  ${CMAKE_SCRIPT_DIR}/Math.cpp
  ${CMAKE_SCRIPT_DIR}/Memory/Array.cpp
  ${CMAKE_SCRIPT_DIR}/Render/OpenGL/CommonRender.cpp
  ${CMAKE_SCRIPT_DIR}/Render/OpenGL/OpenGLWrapper.cpp
  ${CMAKE_SCRIPT_DIR}/Render/Shader.cpp
  ${CMAKE_SCRIPT_DIR}/STD.cpp
  ${CMAKE_SCRIPT_DIR}/Scene/Scene.cpp
  ${CMAKE_SCRIPT_DIR}/Scene/SceneObject.cpp
  ${CMAKE_SCRIPT_DIR}/ThreadPool.cpp
)
set (ENGINE_PCH_HEADER
  ${CMAKE_SCRIPT_DIR}/EnginePCH.hpp
)
set (EDITOR_SRC
  ${CMAKE_SCRIPT_DIR}/Editor/Editor.cpp
  ${CMAKE_SCRIPT_DIR}/Editor/EditorLevel.cpp
  ${CMAKE_SCRIPT_DIR}/Entity/EditorGrid.cpp
)
if (WIN32)
  list (APPEND ENGINE_SRC
    ${CMAKE_SCRIPT_DIR}/Render/glad/wgl.c
    ${CMAKE_SCRIPT_DIR}/Render/OpenGL/WinRender.cpp
    ${CMAKE_SCRIPT_DIR}/Render/glad/gl.c
  )
elseif (LINUX)
  list (APPEND ENGINE_SRC
    ${CMAKE_SCRIPT_DIR}/Render/OpenGL/X11Render.cpp
    ${CMAKE_SCRIPT_DIR}/Render/glad/glx.c
    ${CMAKE_SCRIPT_DIR}/Render/glad/gl.c
  )
  list (APPEND EDITOR_SRC
    ${CMAKE_SCRIPT_DIR}/Render/OpenGL/imgui_impl_x11.cpp
  )
  if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    list (APPEND ENGINE_SRC
      ${CMAKE_SCRIPT_DIR}/Render/OpenGL/imgui_impl_x11.cpp
    )
  endif()
elseif (APPLE)
  list (APPEND ENGINE_SRC
    # Add your Objective-C and C++ source files
    ${CMAKE_SCRIPT_DIR}/Render/OpenGL/NSRender.mm
  )
  # Set the compiler flags for Objective-C and C++
  #set_source_files_properties(${ENGINE_SRC} PROPERTIES
  #  COMPILE_FLAGS "-x objective-c++"
endif()

function(make_library TARGET)
  target_include_directories(${TARGET} PUBLIC ${ENGINE_INCLUDE_DIRS})

  target_link_libraries(${TARGET} PRIVATE ${OPENGL_LIBRARIES})
  if (LINUX)
    target_link_libraries(${TARGET} PRIVATE X11)
  elseif(APPLE)
    target_include_directories(${TARGET} PRIVATE /usr/local/include)
    target_link_libraries(${TARGET} PRIVATE
      "-framework CoreFoundation"
      "-framework CoreGraphics"
      "-framework AppKit"
      "-framework OpenGL"
      "-framework Cocoa"
      "-framework AudioToolbox"
      "-framework CoreAudio"
    )
    target_compile_definitions(${TARGET} PUBLIC GL_SILENCE_DEPRECATION)
  endif()

  # target_compile_options(${TARGET} PRIVATE -fsanitize=address)
  # target_link_options(${TARGET} PRIVATE -fsanitize=address)

  target_include_directories(${TARGET} SYSTEM PRIVATE ${CMAKE_SCRIPT_DIR}/Dependencies/freetype/include)
  target_link_libraries(${TARGET} PRIVATE freetype)

  set_target_properties(${TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Engine"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Engine"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Engine"
  )

  target_compile_options(${TARGET} PRIVATE
    # wd4100 - MSVC silence unused parameters
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4100>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wno-unused-parameter>
  )

  if (LINUX)
    add_custom_command(TARGET ${TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "/usr/lib/libsndfile-custom/libsndfile.so"
        $<TARGET_FILE_DIR:${TARGET}>/libsndfile.so.1)
  elseif (APPLE)
    # Set the MACOSX_BUNDLE flag to create a macOS app bundle
    #set_target_properties(${TARGET} PROPERTIES
    #  MACOSX_BUNDLE TRUE
    # MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    #)
    add_custom_command(TARGET ${TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "/usr/local/opt/libsndfile-custom/libsndfile.dylib"
        $<TARGET_FILE_DIR:${TARGET}>/libsndfile.1.dylib)
    set_target_properties(SndFile::sndfile PROPERTIES
      INSTALL_NAME_DIR "@executable_path"
      INSTALL_NAME "@loader_path/libsndfile.1.dylib"
    )
    set_target_properties(${TARGET} PROPERTIES
      BUILD_WITH_INSTALL_RPATH TRUE
      INSTALL_RPATH "@executable_path"
    )
  endif()

  add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SCRIPT_DIR}/LICENSE.txt"
      $<TARGET_FILE_DIR:${TARGET}>)

  if (LINUX)
    target_link_libraries(${TARGET} PRIVATE ALSA::ALSA)
    target_link_libraries(${TARGET} PRIVATE SndFile::sndfile)
  elseif (APPLE)
    target_link_libraries(${TARGET} PRIVATE /usr/local/opt/libsndfile-custom/libsndfile.dylib)
  elseif (WIN32)
    target_link_libraries(${TARGET} PRIVATE SndFile::sndfile)
  endif()

endfunction()

# add_library(${TARGET_ENGINE} STATIC ${ENGINE_SRC})
# add_library(${TARGET_ENGINE_EDITOR} STATIC ${ENGINE_SRC} ${EDITOR_SRC})

function(link_engine TARGET)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_link_libraries(${TARGET} PRIVATE imgui)
  endif()
  make_library(${TARGET})
endfunction()

function(link_engine_editor TARGET)
  if (MSVC)
    target_compile_definitions(${TARGET} PUBLIC EDITOR DEBUG)
  else()
    target_compile_definitions(${TARGET} PUBLIC -DEDITOR -DDEBUG)
  endif()

  target_link_libraries(${TARGET} PRIVATE imgui)

  if (APPLE)
    target_link_libraries(${TARGET} PRIVATE
      "-framework GameController"
    )
  endif()
      
  make_library(${TARGET})
endfunction()

