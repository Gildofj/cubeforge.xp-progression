# Create an OBJECT library for shared core files to avoid recompilation in tests
add_library(PyroProgression_Common OBJECT
    utility.cpp
    features/drops/DropSystem.cpp
    features/hud/HUDFormatter.cpp
    features/network/NetworkSync.cpp
    features/progression/ProgressionSystem.cpp
    features/scaling/ScalingSystem.cpp
)
add_library(Pyro::Common ALIAS PyroProgression_Common)

# Link to common compiler options, standard, and CWSDK
target_link_libraries(PyroProgression_Common PUBLIC CWSDK Pyro::CompilerFlags)

# Local include directories
target_include_directories(PyroProgression_Common PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/features>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/memory>
)

# External/Third-party include directories (marked SYSTEM to suppress warnings)
target_include_directories(PyroProgression_Common SYSTEM PUBLIC 
    ${cubeforge_sdk_SOURCE_DIR}
)

if(MSVC)
    set_property(TARGET PyroProgression_Common PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_definitions(PyroProgression_Common PRIVATE _ITERATOR_DEBUG_LEVEL=0 MODLOADER=1 CWSDK_NO_GLOBAL_NEW_DELETE=1)
endif()

# Enable Precompiled Headers to speed up compilation
target_precompile_headers(PyroProgression_Common PUBLIC
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
        add_library(PyroProgression SHARED 
            $<TARGET_OBJECTS:PyroProgression_Common>
            main.cpp
            trampolines.asm
        )
    else()
        add_library(PyroProgression SHARED 
            $<TARGET_OBJECTS:PyroProgression_Common>
            main.cpp
        )
    endif()
    add_library(Pyro::Mod ALIAS PyroProgression)
    add_library(Pyro::PyroProgression ALIAS PyroProgression)

    # Local include directories
    target_include_directories(PyroProgression PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/features
        ${CMAKE_CURRENT_SOURCE_DIR}/memory
    )

    # External/Third-party include directories
    target_include_directories(PyroProgression SYSTEM PRIVATE
        ${cubeforge_sdk_SOURCE_DIR}
    )

    target_link_libraries(PyroProgression PRIVATE 
        CWSDK 
        Pyro::CompilerFlags
    )

    # Reuse Precompiled Headers from PyroProgression_Common
    target_precompile_headers(PyroProgression REUSE_FROM PyroProgression_Common)

    if(MSVC)
        set_property(TARGET PyroProgression PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    # Automatically synchronize built artifact to dist/ directory
    add_custom_command(TARGET PyroProgression POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/dist"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:PyroProgression>
            "${CMAKE_SOURCE_DIR}/dist/PyroProgression.dll"
        COMMENT "Synchronizing PyroProgression.dll to dist/"
    )
endif()
