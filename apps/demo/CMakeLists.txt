
add_executable (demo main.cpp 
				components/game_piece.cpp components/game_piece_loader.cpp 
				components/board.cpp components/board_loader.cpp 
				components/board_square.cpp components/board_square_loader.cpp
				components/card.cpp components/card_loader.cpp
				components/dice.cpp components/dice_loader.cpp
				components/player.cpp components/player_loader.cpp
				components/board_path_movement.cpp components/board_path_movement_loader.cpp
				components/hit_points.cpp
				util/boardgen.cpp)

target_link_libraries(demo PRIVATE
	ecs
	transforms
	rendering
	glbinding
	imgui
	audio
    core
	asset
	collisions
	physics
	engineui)

add_custom_target(copy_demo_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

add_dependencies (demo copy_demo_assets)

if (UNIX)
    target_link_libraries(demo PRIVATE PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(demo PRIVATE ${CMAKE_DL_LIBS})
endif ()

set_target_properties(demo PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(demo)
