# Preamble
cmake_minimum_required(VERSION 3.12)
project(WizardEngine C CXX)

# Project wide setup
set(WizardEngine_VERSION_MAJOR 0)
set(WizardEngine_VERSION_MINOR 1)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/installer/GAMEDIRECTORY")
set(EXTERNAL_INSTALL_LOCATION "${PROJECT_SOURCE_DIR}/external")
set(OpenGL_GL_PREFERENCE "GLVND")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
set(COTIRE_MINIMUM_NUMBER_OF_TARGET_SOURCES 5)

# Hide the console window in visual studio projects
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
endif()


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
add_subdirectory(apps/prototype_one)
add_subdirectory(apps/boardgen)

# Tests
#add_subdirectory(test)

# Packaging
include(GNUInstallDirs)
INSTALL(TARGETS warigami COMPONENT warigami)
INSTALL(TARGETS warigami glbinding COMPONENT warigami)
INSTALL(FILES ${FMOD_SO_FILE} ${FREETYPE_SO_FILE} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT warigami)
INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/bin/assets DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT warigami)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/installer/GAMEDIRECTORY/icon.ico DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT warigami)

#include (cmake/CPackBuild.cmake)

