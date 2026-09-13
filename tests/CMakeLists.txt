set(TEST_SOURCES
    main.cpp
    unit/test_display_formatting.cpp
    unit/test_memory_helper.cpp
    unit/test_network_sync.cpp
    unit/test_progression_formulas.cpp
    unit/test_stat_scaling.cpp
)

add_executable(pyro_tests 
    ${TEST_SOURCES}
)

# Link against the Common object library, SDK, and compiler options
target_link_libraries(pyro_tests PRIVATE 
    Pyro::Common 
    Pyro::CompilerFlags
    CWSDK
)

# Include directories
target_include_directories(pyro_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/features
    ${CMAKE_SOURCE_DIR}/src/memory
)

target_include_directories(pyro_tests SYSTEM PRIVATE
    ${cubeforge_sdk_SOURCE_DIR}
)

if(MSVC)
    set_property(TARGET pyro_tests PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Reuse precompiled headers from PyroProgression_Common
target_precompile_headers(pyro_tests REUSE_FROM PyroProgression_Common)

include(CTest)
enable_testing()
add_test(NAME AllTests COMMAND pyro_tests)
