# ecs
add_executable(ecs_tests ecs/ecs_tests.cpp)
target_link_libraries(ecs_tests ecs)
target_include_directories(ecs_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(ecs_tests PUBLIC cxx_std_17)

# event
add_executable(event_tests event/event_tests.cpp)
target_link_libraries(event_tests event)
target_include_directories(event_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(event_tests PUBLIC cxx_std_17)

# lua
add_executable(lua_test lua/test.c)
target_link_libraries(lua_test luajit)
target_compile_features(lua_test PUBLIC cxx_std_17)

add_custom_target(copy_script
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lua/script.lua ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/script.lua
)

add_dependencies (lua_test copy_script)


# asset
add_executable(asset_tests asset/main.cpp asset/test_actual_loaders.cpp)
target_link_libraries(asset_tests asset transforms rendering assimp)
target_include_directories(asset_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(asset_tests PUBLIC cxx_std_17)

add_custom_target(copy_test_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/asset/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

add_dependencies (asset_tests copy_test_assets)

##### RENDER ASSET #####
add_executable(render_asset_test render_asset/main.cpp)
target_link_libraries(render_asset_test 
    asset
    core
    rendering
    transforms)
target_compile_features(render_asset_test PUBLIC cxx_std_17)
add_custom_target(render_asset_test_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/render_asset/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
add_dependencies (render_asset_test render_asset_test_assets)


##### AUDIO #####
add_executable(audio_tests audio/main.cpp)
target_link_libraries(audio_tests asset core audio ecs transforms rendering)
target_compile_features(audio_tests PUBLIC cxx_std_17)

add_custom_target(audio_test_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/audio/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
add_dependencies (audio_tests audio_test_assets)

##### SKELETAL #####
add_executable(skeletal_test skeletal/skeletal.cpp)
target_link_libraries(skeletal_test
        asset
        core
        audio
        rendering
        transforms)
target_compile_features(skeletal_test PUBLIC cxx_std_17)
add_custom_target(skeletal_test_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/skeletal/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
add_dependencies (skeletal_test skeletal_test_assets)

##### COLLISIONS #####
add_executable(collision_tests collisions/collisions_test.cpp)
target_include_directories(collision_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(collision_tests collisions glm)
