# Preamble
cmake_minimum_required(VERSION 3.14)
project(WizardEngine C CXX)

# Project wide setup
set(WizardEngine_VERSION_MAJOR 0)
set(WizardEngine_VERSION_MINOR 1)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install")
set(EXTERNAL_INSTALL_LOCATION "${PROJECT_SOURCE_DIR}/external")
set(OpenGL_GL_PREFERENCE "GLVND")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)

# Dependencies
include(add_glfw)
include(add_glbinding)
include(add_nlohmann_json)
include(add_glm)
add_subdirectory(thirdparty/imgui)

# Main build targets
add_subdirectory(src)
add_subdirectory(apps/game)

# Tests
add_subdirectory(test)

# Packaging
INSTALL(TARGETS game COMPONENT game)
include (cmake/CPackBuild.cmake)
