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
add_executable(asset_tests asset/main.cpp)
target_link_libraries(asset_tests asset)
target_include_directories(asset_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(asset_tests PUBLIC cxx_std_17)

