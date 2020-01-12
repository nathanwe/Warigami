include(FetchContent)

set (OPTION_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set (OPTION_BUILD_TESTS OFF CACHE INTERNAL "")
set (OPTION_BUILD_DOCS OFF CACHE INTERNAL "")
set (OPTION_BUILD_TOOLS OFF CACHE INTERNAL "")


FetchContent_Declare(glbinding
    PREFIX glbinding
    GIT_REPOSITORY https://github.com/cginternals/glbinding.git
    GIT_TAG v3.1.0
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glbinding)
