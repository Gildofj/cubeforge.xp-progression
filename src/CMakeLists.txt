# Create an OBJECT library for shared core files to avoid recompilation in tests
add_library(xp_progression_common OBJECT
    utility.cpp
    features/drops/DropSystem.cpp
    features/hud/HUDFormatter.cpp
    features/network/NetworkSync.cpp
    features/progression/ProgressionSystem.cpp
    features/scaling/ScalingSystem.cpp
)
add_library(XPProgression::Common ALIAS xp_progression_common)

# Link to common compiler options, standard, and CWSDK
target_link_libraries(xp_progression_common PUBLIC CWSDK XPProgression::CompilerFlags)

# Local include directories
target_include_directories(xp_progression_common PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/features>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/memory>
)

# External/Third-party include directories (marked SYSTEM to suppress warnings)
target_include_directories(xp_progression_common SYSTEM PUBLIC 
    ${cubeforge_sdk_SOURCE_DIR}
)

if(MSVC)
    set_property(TARGET xp_progression_common PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_definitions(xp_progression_common PRIVATE _ITERATOR_DEBUG_LEVEL=0 MODLOADER=1 CWSDK_NO_GLOBAL_NEW_DELETE=1)
endif()

# Enable Precompiled Headers to speed up compilation
target_precompile_headers(xp_progression_common PUBLIC
    <vector>
    <string>
    <unordered_map>
    <memory>
    <deque>
    <algorithm>
    <cmath>
    <windows.h>
)

if(BUILD_MOD)
    if(MSVC)
        set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/trampolines.asm" PROPERTIES LANGUAGE ASM_MASM)
        add_library(xp-progression SHARED 
            $<TARGET_OBJECTS:xp_progression_common>
            main.cpp
            trampolines.asm
        )
    else()
        add_library(xp-progression SHARED 
            $<TARGET_OBJECTS:xp_progression_common>
            main.cpp
        )
    endif()
    add_library(xp_progression ALIAS xp-progression)
    add_library(XPProgression::Mod ALIAS xp-progression)
    add_library(CubeForge::XPProgression ALIAS xp-progression)

    # Local include directories
    target_include_directories(xp-progression PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/features
        ${CMAKE_CURRENT_SOURCE_DIR}/memory
    )

    # External/Third-party include directories
    target_include_directories(xp-progression SYSTEM PRIVATE
        ${cubeforge_sdk_SOURCE_DIR}
    )

    target_link_libraries(xp-progression PRIVATE 
        CWSDK 
        XPProgression::CompilerFlags
    )

    # Reuse Precompiled Headers from xp_progression_common
    target_precompile_headers(xp-progression REUSE_FROM xp_progression_common)

    if(MSVC)
        set_property(TARGET xp-progression PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    # Automatically synchronize built artifact to dist/ directory
    add_custom_command(TARGET xp-progression POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/dist"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:xp-progression>
            "${CMAKE_SOURCE_DIR}/dist/xp-progression.dll"
        COMMENT "Synchronizing xp-progression.dll to dist/"
    )
endif()
