add_executable(ecs_tests ecs/ecs_tests.cpp)
target_link_libraries(ecs_tests ecs)
target_include_directories(ecs_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(ecs_tests PUBLIC cxx_std_17)
