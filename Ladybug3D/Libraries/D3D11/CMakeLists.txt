# Target name
set(target Ladybug3D_D3D11)

# Link Library
link_libraries(d3d11)
link_libraries(DXGI)

# Sources
file(GLOB headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
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