# ================================
# CMake Build Settings
# ================================
# This CMakeLists.txt defines the build configuration for the C++ project.
# It configures C++20 support, exports compile commands for clangd, automatically
# includes source files, compiles shaders, fetches third-party libraries, 
# and defines platform and build configuration macros.

cmake_minimum_required(VERSION 3.25)

# Set the application name and version
project(cpp-advanced-gui-template VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ Language Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export to compile_commands.json (for clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force Clang to show coloured diagnostics output
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fcolor-diagnostics)
endif()

# Fetch Third Party Libraries
include(FetchContent)

# GLFW
set(glfw_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/glfw")
set(glfw_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/External/glfw")
set(glfw_SUBBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/External/glfw-subbuild")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_PROGRESS   TRUE
    SOURCE_DIR     ${glfw_SOURCE_DIR}
    BINARY_DIR     ${glfw_BINARY_DIR}
    SUBBUILD_DIR   ${glfw_SUBBUILD_DIR}
)

# Disable GLFW examples, tests, and documentation
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(glfw)

# Dear ImGui
set(imgui_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui")
set(imgui_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/External/imgui")
set(imgui_SUBBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/External/imgui-subbuild")

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.8-docking
    GIT_PROGRESS   TRUE
    SOURCE_DIR     ${imgui_SOURCE_DIR}
    BINARY_DIR     ${imgui_BINARY_DIR}
    SUBBUILD_DIR   ${imgui_SUBBUILD_DIR}
)
FetchContent_MakeAvailable(imgui)

# Find system Vulkan package
find_package(Vulkan REQUIRED)

# Add Source files
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "Source/*.cpp")
set(IMGUI_SRC 
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
)

# Create Executable
add_executable(cpp-advanced-gui-template
    ${SRC}
    ${IMGUI_SRC}
)

# Add Include Directories
target_include_directories(cpp-advanced-gui-template PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/"
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)

# Link Libraries
target_link_libraries(cpp-advanced-gui-template PRIVATE 
    glfw 
    Vulkan::Vulkan
)

# Link macOS Required Libraries (macOS Only)
if(APPLE)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(COREVIDEO CoreVideo)
    target_link_libraries(cpp-advanced-gui-template PRIVATE ${COCOA} ${IOKIT} ${COREVIDEO})
endif()

# Find GLSL Shader Compiler
if(NOT Vulkan_GLSLC_EXECUTABLE)
    find_program(Vulkan_GLSLC_EXECUTABLE glslc)
endif()

# Add GLSL Shader Files
file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.comp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.tesc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.tese"
    "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders/*.geom"
)

# Compile GLSL Shaders to SPIR-V
set(SHADER_OUTPUTS)

foreach(SHADER_SOURCE ${SHADER_SOURCES})
    file(RELATIVE_PATH SHADER_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Data/Shaders" "${SHADER_SOURCE}")

    set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Data/Shaders/${SHADER_RELATIVE_PATH}.spv")

    get_filename_component(SHADER_OUTPUT_DIRECTORY "${SHADER_OUTPUT}" DIRECTORY)

    add_custom_command(
        OUTPUT "${SHADER_OUTPUT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_OUTPUT_DIRECTORY}"
        COMMAND "${Vulkan_GLSLC_EXECUTABLE}" "${SHADER_SOURCE}" -o "${SHADER_OUTPUT}"
        DEPENDS "${SHADER_SOURCE}"
        COMMENT "Compiling shader ${SHADER_RELATIVE_PATH}..."
        VERBATIM
    )

    list(APPEND SHADER_OUTPUTS "${SHADER_OUTPUT}")
endforeach()

add_custom_target(shaders ALL DEPENDS ${SHADER_OUTPUTS})
add_dependencies(cpp-advanced-gui-template shaders)

# Define Platform Macros
if(WIN32)
    target_compile_definitions(cpp-advanced-gui-template PRIVATE WINDOWS)
elseif(APPLE)
    target_compile_definitions(cpp-advanced-gui-template PRIVATE MAC)
elseif(LINUX)
    target_compile_definitions(cpp-advanced-gui-template PRIVATE LINUX)
endif()

# Define Configuration Macros
target_compile_definitions(cpp-advanced-gui-template PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:RelWithDebInfo>:RELEASE>
    $<$<CONFIG:Release>:RELEASE>
)