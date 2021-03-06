# Target name
set(target Ladybug3D_D3D12)

#include
include_directories(${THIRD_PARTY_DIRECTORY})

# Link Library
link_libraries(d3d12)
link_libraries(d3dcompiler)
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

target_compile_options(${target} PUBLIC 
    /wd4267     # Cast size_t type to smaller size type warning
    /wd6001     # Uninitialized Local Variable Warning
    /wd26495    # Uninitialized Member Variable Warning
    /wd26812    # Unscoped Enum Warning
    /wd26451    # Arithmetic overflow Warning
)