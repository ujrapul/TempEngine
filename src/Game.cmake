# SPDX-FileCopyrightText: 2023 Ujwal Vujjini
# SPDX-License-Identifier: MIT

if (APPLE)
  include (../Apple.cmake)
endif()

function(make_executable TARGET OUTPUT_NAME)
  set(TARGET_INCLUDE_DIR
    Components
    Entity
    Render
    Scene
    LevelSerializer
    UT
    .
  )
  target_include_directories(${TARGET} PRIVATE ${TARGET_INCLUDE_DIR})

  if (APPLE)
    make_xcode(${TARGET})
  endif()

  set_target_properties(${TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}"
  )

  target_compile_options(${TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
  )

  if (WIN32)
    add_custom_command(TARGET ${TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LUAJIT_SOURCE_DIR}/lua51.dll"
        $<TARGET_FILE_DIR:${TARGET}>)
    add_custom_command(TARGET ${TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/libsndfile/bin/sndfile.dll"
        $<TARGET_FILE_DIR:${TARGET}>)
  endif()
endfunction()

function(make_executable_editor TARGET OUTPUT_NAME)
  set(TARGET_EDITOR_INCLUDE_DIR
    Editor
  )
  target_include_directories(${TARGET} PRIVATE ${TARGET_EDITOR_INCLUDE_DIR})
  make_executable(${TARGET} ${OUTPUT_NAME})
endfunction()
