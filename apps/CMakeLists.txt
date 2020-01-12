add_executable (game game.cpp)

target_link_libraries(game
    glfw
    core
    glbinding)
