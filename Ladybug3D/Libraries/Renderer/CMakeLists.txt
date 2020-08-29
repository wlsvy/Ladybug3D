# Target name
set(target Ladybug3D_Renderer)

#include / link
include_directories(${LIBRARY_DIRECTORY})
include_directories(${THIRD_PARTY_DIRECTORY})

# Link Library
link_libraries(d3d12)
link_libraries(DXGI)
link_libraries(imgui)
link_libraries(Ladybug3D_D3D12)

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

#Add Definition
target_compile_definitions(Ladybug3D_Renderer PRIVATE
    NOMINMAX    
	LADYBUG3D_ROOT_PATH="${BASE_DRIECTORY}"
	LADYBUG3D_RESOURCE_PATH="${RESOURCE_DRIECTORY}")