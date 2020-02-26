include(cotire)

add_executable (game game.cpp)

target_link_libraries(game PRIVATE    
    asset
    ecs
    transforms
    rendering
    glbinding
    imgui)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

add_dependencies (game copy_assets)

if (UNIX)
    target_link_libraries(game PRIVATE PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(game PRIVATE ${CMAKE_DL_LIBS})
endif ()



cotire(game)