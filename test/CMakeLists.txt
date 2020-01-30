# ecs
add_executable(ecs_tests ecs/ecs_tests.cpp)
target_link_libraries(ecs_tests ecs)
target_include_directories(ecs_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(ecs_tests PUBLIC cxx_std_17)


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

##### RENDERING #####
add_executable(rendering_test rendering/test.cpp)
target_link_libraries(rendering_test 
    core
    rendering
    transforms)
target_compile_features(rendering_test PUBLIC cxx_std_17)

add_custom_target(rendering_test_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/rendering/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)
add_dependencies (rendering_test rendering_test_assets)


##### AUDIO #####
add_executable(audio_tests audio/main.cpp)
target_link_libraries(audio_tests audio ecs transforms)
target_compile_features(audio_tests PUBLIC cxx_std_17)

add_custom_target(audio_test_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/audio/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
add_dependencies (audio_tests audio_test_assets)
