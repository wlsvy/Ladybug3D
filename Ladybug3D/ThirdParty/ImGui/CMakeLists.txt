# Target name
set(target imgui)

# Sources
file(GLOB headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Build library
add_library(${target}
    ${sources})

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
)