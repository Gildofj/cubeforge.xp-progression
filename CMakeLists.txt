cmake_minimum_required (VERSION 3.15)
project(project_PyroProgression LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CWSDK/CMakeLists.txt")
    add_subdirectory(CWSDK)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../CWSDK/CMakeLists.txt")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../CWSDK" "${CMAKE_CURRENT_BINARY_DIR}/CWSDK")
endif()

set(PYRO_CORE_SOURCES
    "src/utility.cpp"
    "src/features/hud/HUDFormatter.cpp"
    "src/features/scaling/ScalingSystem.cpp"
    "src/features/progression/ProgressionSystem.cpp"
    "src/features/network/NetworkSync.cpp"
    "src/features/drops/DropSystem.cpp"
)

add_library (PyroProgression SHARED 
    "main.cpp" 
    "src/XPOverwrite.h"  
    "src/LevelDisplayOverwrite.h" 
    "src/utility.h" 
    "src/GearScalingOverWrite.h" 
    "src/GoldDropOverWrite.h" 
    "src/RegionTextDrawOverwrite.h"
    ${PYRO_CORE_SOURCES}
)

target_link_libraries (PyroProgression LINK_PUBLIC CWSDK)
target_include_directories (PyroProgression PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

option(BUILD_TESTING "Build PyroProgression test suite" ON)

if(BUILD_TESTING)
    enable_testing()
    file(GLOB_RECURSE PYRO_TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
    )

    add_executable(pyro_tests 
        ${PYRO_TEST_SOURCES}
        ${PYRO_CORE_SOURCES}
    )
    target_link_libraries(pyro_tests PRIVATE CWSDK)
    target_include_directories(pyro_tests PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )

    add_test(NAME pyro_tests COMMAND pyro_tests)
endif()