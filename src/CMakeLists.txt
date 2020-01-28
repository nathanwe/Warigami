#TODO: Probably split up this cmake file into one file per library


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
            nlohmann_json
            glm        
            platform
            glfw)


##### MEMORY #####
set(MEMORY_HEADER_LIST
    ${PROJECT_SOURCE_DIR}/include/memory/alignment.hpp
    ${PROJECT_SOURCE_DIR}/include/memory/pool_allocator.hpp
    ${PROJECT_SOURCE_DIR}/include/memory/pool_memory_block.hpp
    ${PROJECT_SOURCE_DIR}/include/memory/typed_pool_allocator.hpp)

set(MEMORY_SOURCE_LIST
    ${PROJECT_SOURCE_DIR}/src/memory/alignment.cpp
    ${PROJECT_SOURCE_DIR}/src/memory/pool_allocator.cpp
    ${PROJECT_SOURCE_DIR}/src/memory/pool_memory_block.cpp)

add_library(memory ${MEMORY_SOURCE_LIST} ${MEMORY_HEADER_LIST})
target_include_directories(memory PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(memory PUBLIC cxx_std_17)


##### ECS #####
set (ECS_SOURCE_LIST
     ${PROJECT_SOURCE_DIR}/src/ecs/archetype_pool.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/archetype_pools.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/state.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/systems.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/world.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/chunk_component_accessor.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/entity.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/component_meta.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/query_cache.cpp)

set (ECS_HEADER_LIST
     ${PROJECT_SOURCE_DIR}/include/ecs/archetype_chunk_component.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/archetype_id.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/archetype_pool.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/chunk_component_accessor.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/component_meta.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/component.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/ecs_types.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/archetype_pools.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/state.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/world.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/entity.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/behavior.hpp
     ${PROJECT_SOURCE_DIR}/include/ecs/register_component.hpp)

add_library(ecs STATIC ${ECS_SOURCE_LIST} ${ECS_HEADER_LIST})
target_include_directories(ecs PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(ecs PUBLIC cxx_std_17)
target_link_libraries(ecs PUBLIC core PRIVATE memory)

##### ASSET #####
set (ASSET_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/asset/scene.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_entity.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/json_cache.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/resource_id.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_hydrater.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/model_loader.cpp)

add_library(asset STATIC ${ASSET_SOURCE_LIST})
target_include_directories(asset PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(asset PUBLIC cxx_std_17)
target_link_libraries(asset PUBLIC core ecs assimp)

##### TRANSFORMS #####
set (TRANSFORMS_SOURCE_LIST
     ${PROJECT_SOURCE_DIR}/src/transforms/transformer.cpp
     ${PROJECT_SOURCE_DIR}/src/transforms/transform_loader.cpp transforms/transform.cpp)

add_library(transforms STATIC ${TRANSFORMS_SOURCE_LIST})
target_include_directories(transforms PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(transforms PUBLIC cxx_std_17)
target_link_libraries(transforms PUBLIC
    ecs
    asset
    glm)
    
##### RENDERING #####
set (RENDERING_SOURCE_LIST
     ${PROJECT_SOURCE_DIR}/src/rendering/asset_cache.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/camera.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/camera_updater.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/loader_camera.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/light_directional.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/light_point.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/loader_light_directional.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/loader_light_point.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/render_loader.cpp 
     ${PROJECT_SOURCE_DIR}/src/rendering/render_pass.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderable_mesh_static.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderer.cpp)

add_library(rendering STATIC ${RENDERING_SOURCE_LIST})
target_include_directories(rendering PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(rendering PUBLIC cxx_std_17)
target_link_libraries(rendering PUBLIC 
    ecs
    asset
    assimp
    glbinding
    glm 
    glfw
    stb_image)
