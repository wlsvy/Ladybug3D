# Target name
set(target imgui)

# Sources
file(GLOB headers
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
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
    /wd26451    # Arithmetic overflow Warning
)