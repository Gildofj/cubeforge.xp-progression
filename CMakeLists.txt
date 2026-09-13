cmake_minimum_required(VERSION 3.25)

if(CMAKE_HOST_WIN32 OR CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    project(xp-progression VERSION "1.0.0" LANGUAGES C CXX ASM_MASM)
else()
    project(xp-progression VERSION "1.0.0" LANGUAGES C CXX)
endif()

# Enforce standard-compliant and modern MSVC runtime behavior
cmake_policy(SET CMP0091 NEW)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Enable compiler cache if available (ccache / sccache)
find_program(CCACHE_PROGRAM NAMES ccache sccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Found compiler cache: ${CCACHE_PROGRAM}")
endif()

# Enable Clang-Tidy integration if requested
option(ENABLE_CLANG_TIDY "Enable static analysis with Clang-Tidy" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_PROGRAM NAMES clang-tidy)
    if(CLANG_TIDY_PROGRAM)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_PROGRAM}")
        message(STATUS "Found Clang-Tidy: ${CLANG_TIDY_PROGRAM}")
    endif()
endif()

# Enable Link-Time Optimization (LTO) for Release/RelWithDebInfo builds if supported
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported LANGUAGES CXX)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
endif()

# Create compiler interface target to enforce standards and common options
add_library(xp_progression_compiler_flags INTERFACE)
add_library(XPProgression::CompilerFlags ALIAS xp_progression_compiler_flags)
add_library(CubeForge::CompilerFlags ALIAS xp_progression_compiler_flags)
target_compile_features(xp_progression_compiler_flags INTERFACE cxx_std_20 c_std_11)

if(MSVC)
    target_compile_options(xp_progression_compiler_flags INTERFACE 
        $<$<COMPILE_LANGUAGE:CXX>:
            /W4          # Stricter warnings (Level 4)
            /EHsc        # Standard C++ exception handling
            /utf-8       # Set Source and Executable character sets to UTF-8
            /permissive- # Strict standards conformance
            /Zc:__cplusplus # Correct __cplusplus macro
            /wd4828      # Disable warning on non-representable characters
            /wd4100      # Suppress unreferenced formal parameter (common in callbacks/hooks)
            /MP          # Multi-process compilation
            /Zc:inline   # Remove unreferenced functions or data
        >
    )
    target_compile_definitions(xp_progression_compiler_flags INTERFACE
        _ITERATOR_DEBUG_LEVEL=0
        _CRT_SECURE_NO_WARNINGS
        DIRECTINPUT_VERSION=0x0800
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        MODLOADER=1
        CWSDK_NO_GLOBAL_NEW_DELETE=1
    )
else()
    target_compile_options(xp_progression_compiler_flags INTERFACE 
        $<$<COMPILE_LANGUAGE:CXX>:
            -Wall 
            -Wextra 
            -Wpedantic 
            -Wno-unused-parameter
        >
    )
endif()

# Temporarily disable Clang-Tidy for external SDK targets
set(OLD_CMAKE_CXX_CLANG_TIDY ${CMAKE_CXX_CLANG_TIDY})
set(CMAKE_CXX_CLANG_TIDY "")

# Fetch and configure cubeforge_sdk
include(FetchContent)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../cubeforge.sdk/CMakeLists.txt")
    FetchContent_Declare(
        cubeforge_sdk
        SYSTEM
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../cubeforge.sdk"
    )
else()
    FetchContent_Declare(
        cubeforge_sdk
        SYSTEM
        GIT_REPOSITORY https://github.com/Gildofj/cubeforge.sdk.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
endif()
FetchContent_MakeAvailable(cubeforge_sdk)

# Restore Clang-Tidy for project targets
set(CMAKE_CXX_CLANG_TIDY ${OLD_CMAKE_CXX_CLANG_TIDY})

option(BUILD_MOD "Build xp-progression shared library / mod DLL" ON)
option(BUILD_TESTING "Build automated unit and regression tests" ON)

# Source subdirectory for mod objects and DLL
add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()