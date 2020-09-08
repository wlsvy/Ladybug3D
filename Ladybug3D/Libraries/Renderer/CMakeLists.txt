# Target name
set(target Ladybug3D_Renderer)

#include / link
include_directories(${LIBRARY_DIRECTORY})
include_directories(${THIRD_PARTY_DIRECTORY})
include_directories(${PRECOMPILED_LIBRARY_DIRECTORY}/Any)
include_directories(${PRECOMPILED_LIBRARY_DIRECTORY}/x64/Debug)
link_directories (${PRECOMPILED_LIBRARY_DIRECTORY}/x64/Debug)

# Link Library
find_library(
  ASSIMP
  NAMES assimp-vc140-mt
  HINTS ${PRECOMPILED_LIBRARY_DIRECTORY}/Any
  REQUIRED)

# Sources
file(GLOB headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE Shaders
    ${RESOURCE_DRIECTORY}/*.hlsl)

# Build library
add_library(${target}
    ${headers}
    ${sources})


target_link_libraries(${target} PUBLIC
    ${ASSIMP}
    imgui
    Ladybug3D_D3D12)



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
)

#Add Definition
target_compile_definitions(Ladybug3D_Renderer PRIVATE
    NOMINMAX    
	LADYBUG3D_ROOT_PATH="${BASE_DRIECTORY}"
	LADYBUG3D_RESOURCE_PATH="${RESOURCE_DRIECTORY}")