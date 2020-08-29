# Target name
set(target Ladybug3D_D3D12)

#include
include_directories(${THIRD_PARTY_DIRECTORY})

# Link Library
link_libraries(d3d12)
link_libraries(Direct12XTK)
link_libraries(DXGI)

# Sources
file(GLOB headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Build library
add_library(${target}
    ${headers}
    ${sources})

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
)