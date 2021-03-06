#TODO: Probably split up this cmake file into one file per library


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


##### EVENT #####
set (EVENT_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/event/event_manager.cpp)

add_library(event ${EVENT_SOURCE_FILES})
target_include_directories(event PUBLIC ${PROJECT_SOURCE_DIR}/include)


##### CORE #####
set (CORE_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/core/framerate.cpp
        ${PROJECT_SOURCE_DIR}/src/core/frame_limiter.cpp
        ${PROJECT_SOURCE_DIR}/src/core/frame_timer.cpp
        ${PROJECT_SOURCE_DIR}/src/core/input_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/core/game_input_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/core/startup_config.cpp
        ${PROJECT_SOURCE_DIR}/src/core/system_info.cpp
        ${PROJECT_SOURCE_DIR}/src/core/cursor_state.cpp
        ${PROJECT_SOURCE_DIR}/src/core/glfw_context.cpp)

add_library(core ${CORE_HEADERS} ${CORE_SOURCE_FILES})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(core
        PUBLIC
            nlohmann_json
            glm        
            platform
            glfw
            event)


##### MEMORY #####
set(MEMORY_SOURCE_LIST
    ${PROJECT_SOURCE_DIR}/src/memory/alignment.cpp
    ${PROJECT_SOURCE_DIR}/src/memory/pool_allocator.cpp
    ${PROJECT_SOURCE_DIR}/src/memory/pool_memory_block.cpp)

add_library(memory ${MEMORY_SOURCE_LIST})
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

add_library(ecs STATIC ${ECS_SOURCE_LIST})
target_include_directories(ecs PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(ecs PUBLIC cxx_std_17)
target_link_libraries(ecs PUBLIC core PRIVATE memory)


##### ASSET #####
set (ASSET_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/asset/asset_manager.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_entity.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/resource_id.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_hydrater.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/model_loader.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/rigged_mesh.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_tracker.cpp)

add_library(asset STATIC ${ASSET_SOURCE_LIST})
target_include_directories(asset PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(asset PUBLIC cxx_std_17)
target_link_libraries(asset PUBLIC core ecs assimp fmod stb_image)


##### TRANSFORMS #####
set (TRANSFORMS_SOURCE_LIST
     ${PROJECT_SOURCE_DIR}/src/transforms/transformer.cpp
     ${PROJECT_SOURCE_DIR}/src/transforms/transform_loader.cpp transforms/transform.cpp)

add_library(transforms STATIC ${TRANSFORMS_SOURCE_LIST})
target_include_directories(transforms PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(transforms PUBLIC cxx_std_17)
target_link_libraries(transforms PUBLIC ecs asset glm)


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
     ${PROJECT_SOURCE_DIR}/src/rendering/loader_model.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/render_loader.cpp 
     ${PROJECT_SOURCE_DIR}/src/rendering/render_pass.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderable_mesh_static.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderable_model_static.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderable_mesh_rigged.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderer.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/render_loader.cpp      
     ${PROJECT_SOURCE_DIR}/src/rendering/camera.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/animator.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/loader_rigged_model.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/skeleton_node.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/skeleton_animation.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/renderable_text.cpp
     ${PROJECT_SOURCE_DIR}/src/rendering/freetype_system.cpp
    ${PROJECT_SOURCE_DIR}/src/rendering/loader_renderable_text.cpp)

add_library(rendering STATIC ${RENDERING_SOURCE_LIST} )
target_include_directories(rendering PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(rendering PUBLIC cxx_std_17)
target_link_libraries(rendering PUBLIC     
    collisions
    ecs
    engineui
    glbinding
    glm 
    glfw
    asset
    freetype)

add_custom_target(copy_render_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/rendering/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/rendering/fonts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/fonts)

add_dependencies (rendering copy_render_assets)

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



##### AUDIO #####
set (AUDIO_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/audio/audio_system.cpp
        ${PROJECT_SOURCE_DIR}/src/audio/audio_listener.cpp
        ${PROJECT_SOURCE_DIR}/src/audio/audio_emitter.cpp ../include/audio/music_player.hpp audio/music_player.cpp)

add_library(audio STATIC ${AUDIO_SOURCE_LIST})
target_include_directories(audio PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(audio PUBLIC cxx_std_17)
target_link_libraries(audio PUBLIC collisions ecs transforms rendering asset fmod glm)







# cotire stuff
#set_target_properties(event PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#set_target_properties(core PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#set_target_properties(memory PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#set_target_properties(ecs PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#set_target_properties(asset PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#set_target_properties(transforms PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#set_target_properties(rendering PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#set_target_properties(engineui PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
#set_target_properties(audio PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)

#cotire(platform)
#cotire(event)
#cotire(core)
#cotire(memory)
#cotire(ecs)
#cotire(asset)
#cotire(transforms)
#cotire(rendering)
#cotire(engineui)
#cotire(audio)

