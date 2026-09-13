set(TEST_SOURCES
    main.cpp
    unit/test_display_formatting.cpp
    unit/test_memory_helper.cpp
    unit/test_network_sync.cpp
    unit/test_progression_formulas.cpp
    unit/test_stat_scaling.cpp
)

add_executable(xp_progression_tests 
    ${TEST_SOURCES}
)

# Link against the Common object library, SDK, and compiler options
target_link_libraries(xp_progression_tests PRIVATE 
    XPProgression::Common 
    XPProgression::CompilerFlags
    CWSDK
)

# Include directories
target_include_directories(xp_progression_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/features
    ${CMAKE_SOURCE_DIR}/src/memory
)

target_include_directories(xp_progression_tests SYSTEM PRIVATE
    ${cubeforge_sdk_SOURCE_DIR}
)

if(MSVC)
    set_property(TARGET xp_progression_tests PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Reuse precompiled headers from xp_progression_common
target_precompile_headers(xp_progression_tests REUSE_FROM xp_progression_common)

include(CTest)
enable_testing()
add_test(NAME AllTests COMMAND xp_progression_tests)
