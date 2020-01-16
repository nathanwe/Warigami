#TODO: Probably split up this cmake file into one file per library



##### ECS #####

set (ECS_HEADERS
    ${PROJECT_SOURCE_DIR}/include/ecs/behavior_base.hpp
    ${PROJECT_SOURCE_DIR}/include/ecs/behaviors.hpp
    ${PROJECT_SOURCE_DIR}/include/ecs/ecs_types.hpp
    ${PROJECT_SOURCE_DIR}/include/ecs/entities.hpp
    ${PROJECT_SOURCE_DIR}/include/ecs/entity.hpp
    ${PROJECT_SOURCE_DIR}/include/ecs/policies/mock_entities.hpp
    ${PROJECT_SOURCE_DIR}/include/ecs/policies/mock_entity.hpp
)

# this is a header-only lib for now.
# note you'll have to change this when cpp files are added.
add_library(ecs INTERFACE)
target_sources(ecs INTERFACE "$<BUILD_INTERFACE:${ECS_HEADERS}>")
target_include_directories(ecs INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(ecs INTERFACE cxx_std_17)



##### PLATFORM #####
set (PLATFORM_HEADERS 
   ${PROJECT_SOURCE_DIR}/include/platform/sleep.hpp
)

set (PLATFORM_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/platform/sleep.cpp
)

add_library(platform ${PLATFORM_HEADERS} ${PLATFORM_SOURCE_FILES})
target_include_directories(platform PRIVATE ${PROJECT_SOURCE_DIR}/include) 


##### CORE #####
set (CORE_HEADERS
        ${PROJECT_SOURCE_DIR}/include/core/framerate.hpp
        ${PROJECT_SOURCE_DIR}/include/core/frame_limiter.hpp
        ${PROJECT_SOURCE_DIR}/include/core/frame_timer.hpp
        ${PROJECT_SOURCE_DIR}/include/core/input_manager.hpp
        ${PROJECT_SOURCE_DIR}/include/core/startup_config.hpp
        ${PROJECT_SOURCE_DIR}/include/core/system_info.hpp)

set (CORE_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/core/framerate.cpp
        ${PROJECT_SOURCE_DIR}/src/core/frame_limiter.cpp
        ${PROJECT_SOURCE_DIR}/src/core/frame_timer.cpp
        ${PROJECT_SOURCE_DIR}/src/core/input_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/core/startup_config.cpp
        ${PROJECT_SOURCE_DIR}/src/core/system_info.cpp)

add_library(core ${CORE_HEADERS} ${CORE_SOURCE_FILES})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(core
        PUBLIC
            ecs
            nlohmann_json
            glm
        PRIVATE
            platform
            glfw
            )
