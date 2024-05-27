# SPDX-FileCopyrightText: 2023 Ujwal Vujjini
# SPDX-License-Identifier: MIT

include (../Engine/Engine.cmake)
include (../Game.cmake)

set ( ENGINE_UT_SOURCE
  UT_Main.cpp
)

add_executable(${TARGET_ENGINE_UNIT_TESTS} ${ENGINE_UT_SOURCE} ${ENGINE_SRC})

# add_dependencies(${TARGET_ENGINE_UNIT_TESTS} ${TARGET_ENGINE})
# target_link_libraries(${TARGET_ENGINE_UNIT_TESTS} ${TARGET_ENGINE})

link_engine(${TARGET_ENGINE_UNIT_TESTS})
if (MSVC)
  target_compile_definitions(${TARGET_ENGINE_UNIT_TESTS} PUBLIC UT)
else()
  target_compile_definitions(${TARGET_ENGINE_UNIT_TESTS} PUBLIC -DUT)
endif()
make_executable(${TARGET_ENGINE_UNIT_TESTS} ${TARGET_ENGINE_UNIT_TESTS})
