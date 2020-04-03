# Preamble
cmake_minimum_required(VERSION 3.12)
project(WizardEngine C CXX)

# Project wide setup
set(WizardEngine_VERSION_MAJOR 0)
set(WizardEngine_VERSION_MINOR 1)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install")
set(EXTERNAL_INSTALL_LOCATION "${PROJECT_SOURCE_DIR}/external")
set(OpenGL_GL_PREFERENCE "GLVND")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
set(COTIRE_MINIMUM_NUMBER_OF_TARGET_SOURCES 5)

# Dependencies
include(add_glfw)
include(add_glbinding)
include(add_nlohmann_json)
include(add_glm)
include(add_assimp)
include(cotire)
add_subdirectory(thirdparty/imgui)
add_subdirectory(thirdparty/lua)
add_subdirectory(thirdparty/fmod)
add_subdirectory(thirdparty/stb)
add_subdirectory(thirdparty/freetype)

# Main build targets
add_subdirectory(src)
add_subdirectory(apps/game)
add_subdirectory(apps/prototype_one)
add_subdirectory(apps/boardgen)


# Tests
add_subdirectory(test)

# Packaging
INSTALL(TARGETS prototype_one COMPONENT prototype_one)
INSTALL(TARGETS prototype_one glbinding COMPONENT prototype_one)
INSTALL(FILES ${FMOD_SO_FILE} ${FREETYPE_SO_FILE} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT prototype_one)
INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/bin/assets DESTINATION / COMPONENT prototype_one)

include (cmake/CPackBuild.cmake)

