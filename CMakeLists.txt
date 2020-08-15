cmake_minimum_required (VERSION 3.8)

project (Ladybug3D)

set(DEFAULT_COMPILE_OPTIONS)
set(DEFAULT_LINKER_OPTIONS)
set(DEFAULT_PROJECT_OPTIONS
    CXX_STANDARD              17 # Not available before CMake 3.8.2; see below for manual command line argument addition
    LINKER_LANGUAGE           "CXX"
    POSITION_INDEPENDENT_CODE ON
)

# 출력 디렉터리를 설정합니다.
set(DEFAULT_CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(SCRIPT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Ladybug3D/Script)
set(LIBRARY_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Ladybug3D/Libraries)
set(THIRD_PARTY_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Ladybug3D/ThirdParty)

# 하위 프로젝트를 포함합니다.
add_subdirectory(${THIRD_PARTY_DIRECTORY}/imgui)
add_subdirectory(${LIBRARY_DIRECTORY}/D3D11)
add_subdirectory(Ladybug3D)

#add_dependencies(Ladybug3D_D3D11, d3d11)
