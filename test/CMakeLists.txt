add_executable(ecs_tests ecs/ecs_tests.cpp)
target_link_libraries(ecs_tests core)
target_include_directories(ecs_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)