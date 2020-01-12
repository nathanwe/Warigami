include(FetchContent)


set (GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set (GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set (GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_Declare(glfw    
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3)


FetchContent_MakeAvailable(glfw)

