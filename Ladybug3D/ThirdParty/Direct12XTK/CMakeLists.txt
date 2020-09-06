# Target name
set(target Direct12XTK)

#include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)

# Sources
file(GLOB_RECURSE headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB sources
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)

# Build library
add_library(${target}
    ${headers}
    ${sources})

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
)