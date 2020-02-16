#TODO: Probably split up this cmake file into one file per library

include(cotire)


##### PLATFORM #####
set (PLATFORM_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/platform/sleep.cpp
    ${PROJECT_SOURCE_DIR}/src/platform/user_data_paths.cpp
    ${PROJECT_SOURCE_DIR}/src/platform/rumble.cpp
)

add_library(platform ${PLATFORM_SOURCE_FILES})
target_include_directories(platform PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(platform PUBLIC
            $<$<BOOL:${WIN32}>:xinput>)

# cotire(platform)


##### EVENT #####
set (EVENT_HEADERS
        ${PROJECT_SOURCE_DIR}/include/event/event.hpp
        ${PROJECT_SOURCE_DIR}/include/event/event_manager.hpp)

set (EVENT_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/event/event_manager.cpp)

add_library(event ${EVENT_HEADERS} ${EVENT_SOURCE_FILES})
target_include_directories(event PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(event PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(event)


##### CORE #####
set (CORE_HEADERS
        ${PROJECT_SOURCE_DIR}/include/core/framerate.hpp
        ${PROJECT_SOURCE_DIR}/include/core/frame_limiter.hpp
        ${PROJECT_SOURCE_DIR}/include/core/frame_timer.hpp
        ${PROJECT_SOURCE_DIR}/include/core/input_manager.hpp
        ${PROJECT_SOURCE_DIR}/include/core/game_input_manager.hpp
        ${PROJECT_SOURCE_DIR}/include/core/startup_config.hpp
        ${PROJECT_SOURCE_DIR}/include/core/system_info.hpp)

set (CORE_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/core/framerate.cpp
        ${PROJECT_SOURCE_DIR}/src/core/frame_limiter.cpp
        ${PROJECT_SOURCE_DIR}/src/core/frame_timer.cpp
        ${PROJECT_SOURCE_DIR}/src/core/input_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/core/game_input_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/core/startup_config.cpp
        ${PROJECT_SOURCE_DIR}/src/core/system_info.cpp
        ${PROJECT_SOURCE_DIR}/src/core/cursor_state.cpp)

add_library(core ${CORE_HEADERS} ${CORE_SOURCE_FILES})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(core
        PUBLIC
            asset
            nlohmann_json
            glm        
            platform
            glfw
            event)

set_target_properties(core PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(core)


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

set_target_properties(memory PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(memory)

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

set_target_properties(ecs PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(ecs)

##### ASSET #####
set (ASSET_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/asset/asset_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_entity.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/resource_id.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_hydrater.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/model_loader.cpp)

add_library(asset STATIC ${ASSET_SOURCE_LIST})
target_include_directories(asset PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(asset PUBLIC cxx_std_17)
target_link_libraries(asset PUBLIC core ecs assimp fmod stb_image)

set_target_properties(asset PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(asset)


##### TRANSFORMS #####
set (TRANSFORMS_SOURCE_LIST
     ${PROJECT_SOURCE_DIR}/src/transforms/transformer.cpp
     ${PROJECT_SOURCE_DIR}/src/transforms/transform_loader.cpp transforms/transform.cpp)

add_library(transforms STATIC ${TRANSFORMS_SOURCE_LIST})
target_include_directories(transforms PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(transforms PUBLIC cxx_std_17)
target_link_libraries(transforms PUBLIC ecs asset glm)

set_target_properties(transforms PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(transforms)





##### RENDERING #####
set (RENDERING_SOURCE_LIST
     ${PROJECT_SOURCE_DIR}/src/rendering/asset_cache.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/camera.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/camera_updater.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/debug_view.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/light_directional.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/light_point.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/loader_camera.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/loader_light_directional.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/loader_light_point.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/render_loader.cpp 
     ${PROJECT_SOURCE_DIR}/src/rendering/render_pass.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderable_mesh_static.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderer.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/render_loader.cpp 
     rendering/renderable_mesh_static.cpp rendering/camera.cpp)

add_library(rendering STATIC ${RENDERING_SOURCE_LIST})
target_include_directories(rendering PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(rendering PUBLIC cxx_std_17)
target_link_libraries(rendering PUBLIC 
    asset
    assimp
    collisions
    ecs
    engineui
    glbinding
    glm 
    glfw
    stb_image)

set_target_properties(rendering PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(rendering)

add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/rendering/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders
)

add_dependencies (rendering copy_shaders)


##### AUDIO #####
set (AUDIO_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/audio/audio_system.cpp
        ${PROJECT_SOURCE_DIR}/src/audio/audio_listener.cpp
        ${PROJECT_SOURCE_DIR}/src/audio/audio_emitter.cpp)

add_library(audio STATIC ${AUDIO_SOURCE_LIST})
target_include_directories(audio PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(audio PUBLIC cxx_std_17)
target_link_libraries(audio PUBLIC ecs transforms rendering asset fmod glm)
set_target_properties(audio PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(audio)


##### ENGINE-UI #####
set (ENGINEUI_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/engine-ui/developer_console.cpp
        ${PROJECT_SOURCE_DIR}/src/engine-ui/entities_view.cpp
        ${PROJECT_SOURCE_DIR}/src/engine-ui/fps_display.cpp
        ${PROJECT_SOURCE_DIR}/src/engine-ui/imgui_overlay.cpp)

add_library(engineui STATIC ${ENGINEUI_SOURCE_LIST})
target_include_directories(engineui PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(engineui PUBLIC cxx_std_17)
target_link_libraries(engineui PUBLIC core event imgui rendering)

set_target_properties(engineui PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(engineui)
##### COLLISIONS #####
set (COLLISIONS_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/collisions/collider.cpp
        ${PROJECT_SOURCE_DIR}/src/collisions/aabb_collider_loader.cpp
        ${PROJECT_SOURCE_DIR}/src/collisions/sphere_collider_loader.cpp
        ${PROJECT_SOURCE_DIR}/src/collisions/collision_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/collisions/rigid_body.cpp
        ${PROJECT_SOURCE_DIR}/src/collisions/rigid_body_loader.cpp)

add_library(collisions STATIC ${COLLISIONS_SOURCE_LIST})
target_include_directories(collisions PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(collisions PUBLIC glm asset transforms)

##### PHYSICS #####
set (PHYSICS_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/physics/physics_update.cpp)

add_library(physics STATIC ${PHYSICS_SOURCE_LIST})
target_include_directories(physics PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(physics PUBLIC glm asset)
target_link_libraries(physics PUBLIC glm asset)
