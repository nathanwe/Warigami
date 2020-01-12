include(FetchContent)

set (GLM_TEST_ENABLE OFF CACHE INTERNAL "")

set (CMAKE_CXX_FLAGS ${CXXFLAGS})

FetchContent_Declare(glm
    PREFIX glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG stable
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(glm)
