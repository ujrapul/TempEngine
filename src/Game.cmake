# SPDX-FileCopyrightText: 2023 Ujwal Vujjini
# SPDX-License-Identifier: MIT

include (../Engine/Engine.cmake)

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
    # wd4100 - MSVC silence unused parameters
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4100>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wno-unused-parameter>
  )

  if (WIN32)
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

function(make_dll GAME_NAME DLL COMMON)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_library(${DLL} SHARED ${DLL}.cpp ${COMMON})
    add_dependencies(${DLL} TempEngine)
    target_link_libraries(${DLL} PRIVATE TempEngine)
    # target_link_libraries(${DLL} PRIVATE EnginePCH)
    target_precompile_headers(${DLL} PRIVATE ${ENGINE_PCH_HEADER})
    target_compile_options(${DLL} PRIVATE -fpic)
    target_include_directories(${DLL} PUBLIC ${ENGINE_INCLUDE_DIRS})

    add_custom_command(                                                                                                                                                                                                                   
      TARGET ${DLL}
      POST_BUILD
      COMMAND touch lock.file
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${GAME_NAME}
      COMMENT "Adding lock.file"
    )
    # link_engine(${DLL})
    make_executable(${DLL} ${GAME_NAME})
  endif()
endfunction()

function(make_game GAME_NAME FILES)
  set(TARGET_GAME ${GAME_NAME})
  set(TARGET_GAME_UT ${GAME_NAME}UT)
  set(TARGET_GAME_EDITOR ${GAME_NAME}Editor)

  set(TARGET_GAME ${TARGET_GAME} PARENT_SCOPE)
  set(TARGET_GAME_UT ${TARGET_GAME_UT} PARENT_SCOPE)
  set(TARGET_GAME_EDITOR ${TARGET_GAME_EDITOR} PARENT_SCOPE)

  if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(GAME_COMMON
      ${FILES}
    )
    set(GAME_COMMON ${GAME_COMMON} PARENT_SCOPE)
  endif()

  set(GAME_SRC
    ${GAME_COMMON}
    main.cpp
  )

  set(GAME_EDITOR_SRC
    # ${GAME_COMMON}
    main.cpp
  )

  set(GAME_UT_SRC
    ${GAME_COMMON}
    UT/main.cpp
  )

  add_library(TempEngine STATIC ${ENGINE_SRC})
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(TempEngine PRIVATE -fpic)
  endif()
  link_engine(TempEngine)
  make_executable(TempEngine ${GAME_NAME})
  target_precompile_headers(TempEngine PRIVATE ${STD_PCH_HEADER})

  add_library(TempEngineEditor STATIC ${EDITOR_SRC})
  add_dependencies(TempEngineEditor TempEngine)
  target_link_libraries(TempEngineEditor PRIVATE TempEngine)
  link_engine_editor(TempEngineEditor)
  make_executable_editor(TempEngineEditor ${GAME_NAME})

  add_executable(${TARGET_GAME} ${GAME_SRC})
  add_executable(${TARGET_GAME_UT} ${GAME_UT_SRC})
  add_executable(${TARGET_GAME_EDITOR} ${GAME_EDITOR_SRC})

  add_dependencies(${TARGET_GAME} TempEngine)
  add_dependencies(${TARGET_GAME_UT} TempEngine)
  add_dependencies(${TARGET_GAME_EDITOR} TempEngineEditor)

  target_link_libraries(${TARGET_GAME} PRIVATE TempEngine)
  target_link_libraries(${TARGET_GAME_UT} PRIVATE TempEngine)
  target_link_libraries(${TARGET_GAME_EDITOR} PRIVATE TempEngineEditor)

  target_compile_definitions(${TARGET_GAME_EDITOR} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/DEDITOR /DDEBUG>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-DEDITOR -DDEBUG>
  )

  # link_engine(${TARGET_GAME})
  # link_engine(${TARGET_GAME_UT})
  # link_engine_editor(${TARGET_GAME_EDITOR})

  make_executable(${TARGET_GAME} ${GAME_NAME})
  make_executable(${TARGET_GAME_UT} ${GAME_NAME})
  make_executable_editor(${TARGET_GAME_EDITOR} ${GAME_NAME})

  if (DLL STREQUAL "ON")
    set_target_properties(${TARGET_GAME} PROPERTIES EXCLUDE_FROM_ALL True)
    set_target_properties(${TARGET_GAME_UT} PROPERTIES EXCLUDE_FROM_ALL True)
    set_target_properties(${TARGET_GAME_EDITOR} PROPERTIES EXCLUDE_FROM_ALL True)
  endif()
endfunction()
