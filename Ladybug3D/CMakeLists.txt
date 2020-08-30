cmake_minimum_required (VERSION 3.8)
# Target name
set(target Ladybug3D)

#include / link
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SCRIPT_DIRECTORY})
include_directories(${LIBRARY_DIRECTORY})
include_directories(${THIRD_PARTY_DIRECTORY})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

link_libraries(imgui)
link_libraries(d3d12)
link_libraries(Ladybug3D_D3D12)
link_libraries(Ladybug3D_Renderer)

# Sources
file(GLOB HeadFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB SourceFiles
	${CMAKE_CURRENT_SOURCE_DIR}/Script/*.cpp)

# Build exe
add_executable(${target}
	${HeadFiles}
	${SourceFiles}
)

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS})
	