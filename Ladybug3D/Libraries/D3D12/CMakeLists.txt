# Target name
set(target Ladybug3D_D3D12)

# Link Library
link_libraries(d3d12)
link_libraries(DXGI)

# Sources
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