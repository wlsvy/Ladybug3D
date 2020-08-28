# Target name
set(target Ladybug3D_Engine)

# Link Library
link_libraries(d3d12)
link_libraries(DXGI)
link_libraries(Ladybug3D_D3D12)

# Sources
file(GLOB headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Build library
# add_dependencies(Ladybug3D_D3D12, Ladybug3D_Engine)
add_library(${target}
    ${headers}
    ${sources})

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
)