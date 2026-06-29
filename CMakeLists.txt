cmake_minimum_required(VERSION 3.16)
project(VulkanProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

# Compiler options
if(MSVC)
    add_compile_options(/O2 /W3)
else()
    add_compile_options(-O2 -Wall)
endif()

# Dependencies
find_package(Vulkan REQUIRED)

# glfw3: CONFIG mode works with vcpkg, Homebrew, and most Linux package managers
find_package(glfw3 CONFIG QUIET)
if(NOT glfw3_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
endif()

# Katra
file(GLOB_RECURSE KATRA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
add_library(katra ${KATRA_SOURCES})
target_include_directories(katra PUBLIC include)
target_link_libraries(katra PUBLIC Vulkan::Vulkan)
if(glfw3_FOUND)
    target_link_libraries(katra PUBLIC glfw)
else()
    target_include_directories(katra PUBLIC ${GLFW_INCLUDE_DIRS})
    target_link_libraries(katra PUBLIC ${GLFW_LIBRARIES})
endif()

add_executable(VulkanProject main.cpp)
target_link_libraries(VulkanProject PRIVATE katra)

# Platform-specific libs
if(APPLE)
    target_link_libraries(VulkanProject PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework QuartzCore"
    )
elseif(UNIX)
    target_link_libraries(VulkanProject PRIVATE
        dl pthread X11 Xxf86vm Xrandr Xi
    )
endif()

# Shader compilation script
if(WIN32)
    set(SHADER_SCRIPT_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/tools/windows_shader_compile.bat")
    set(SHADER_SCRIPT_DEST "${CMAKE_CURRENT_BINARY_DIR}/compile.bat")
else()
    set(SHADER_SCRIPT_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/tools/unix_shader_compile.sh")
    set(SHADER_SCRIPT_DEST "${CMAKE_CURRENT_BINARY_DIR}/compile.sh")
endif()

configure_file(${SHADER_SCRIPT_SRC} ${SHADER_SCRIPT_DEST} COPYONLY)

# Shader GLSL sources copy and track for rebuilds
file(GLOB SHADER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
)

set(SHADER_OUTPUTS "")
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SHADER_OUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}")
    add_custom_command(
        OUTPUT  ${SHADER_OUT}
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${SHADER_OUT}
        DEPENDS ${SHADER}
        COMMENT "Copying shader: ${SHADER_NAME}"
    )
    list(APPEND SHADER_OUTPUTS ${SHADER_OUT})
endforeach()

# Texture copy
file(GLOB TEXTURE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/textures/*"
)

set(TEXTURE_OUTPUTS "")
foreach(TEXTURE ${TEXTURE_SOURCES})
    get_filename_component(TEXTURE_NAME ${TEXTURE} NAME)
    set(TEXTURE_OUT "${CMAKE_CURRENT_BINARY_DIR}/textures/${TEXTURE_NAME}")
    add_custom_command(
        OUTPUT  ${TEXTURE_OUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/textures"
        COMMAND ${CMAKE_COMMAND} -E copy ${TEXTURE} ${TEXTURE_OUT}
        DEPENDS ${TEXTURE}
        COMMENT "Copying texture: ${TEXTURE_NAME}"
    )
    list(APPEND TEXTURE_OUTPUTS ${TEXTURE_OUT})
endforeach()

# Shader compilation
file(GLOB SHADER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
)

# Find glslc (comes with the Vulkan SDK)
find_program(GLSLC glslc HINTS
    "$ENV{VULKAN_SDK}/bin"
    "/usr/bin"
    "/usr/local/bin"
)
if(NOT GLSLC)
    message(FATAL_ERROR "glslc not found. Make sure the Vulkan SDK is installed.")
endif()

set(SPIRV_OUTPUTS "")
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV_OUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT  ${SPIRV_OUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${GLSLC} ${SHADER} -o ${SPIRV_OUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader: ${SHADER_NAME}"
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV_OUT})
endforeach()

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

add_library(glm_h INTERFACE)
target_include_directories(glm_h INTERFACE ${glm_SOURCE_DIR})
target_link_libraries(katra PUBLIC glm_h)

# stb
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG 31c1ad37456438565541f4919958214b6e762fb4
)
FetchContent_MakeAvailable(stb)

add_library(stb_h INTERFACE)
target_include_directories(stb_h INTERFACE ${stb_SOURCE_DIR})
target_link_libraries(katra PUBLIC stb_h)

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_OUTPUTS})
add_dependencies(VulkanProject CompileShaders)

add_custom_target(CopyShaders ALL DEPENDS ${SHADER_OUTPUTS})
add_dependencies(VulkanProject CopyShaders)

add_custom_target(CopyTextures ALL DEPENDS ${TEXTURE_OUTPUTS})
add_dependencies(VulkanProject CopyTextures)