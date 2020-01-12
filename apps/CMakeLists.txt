add_executable (game game.cpp)

target_link_libraries(game
    glfw
    core
    glbinding)

if (UNIX)
    target_link_libraries(game PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(game PRIVATE ${CMAKE_DL_LIBS})
endif ()