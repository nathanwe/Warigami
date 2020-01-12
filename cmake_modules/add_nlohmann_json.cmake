include(FetchContent)

set (JSON_BuildTests OFF  CACHE INTERNAL "")


FetchContent_Declare(nlohmann_json
    PREFIX nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.7.2
)

FetchContent_MakeAvailable(nlohmann_json)
