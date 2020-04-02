
add_executable (prototype_one
	board_update_system.hpp
	combat.cpp
	components/board_loader.cpp
	components/board_square_loader.cpp
	components/board_square.cpp
	components/board_square.cpp components/board_square_loader.cpp
	components/board.cpp
	components/board.cpp components/board_loader.cpp
	components/card_enum.hpp
	components/card_loader.cpp
	components/card.cpp
	components/card.cpp components/card_loader.cpp
	components/countdown.cpp
	components/deck_option.cpp
	components/deck_selection.cpp
	components/deck_ui.cpp
	components/deck_ui.cpp
	components/deck_ui.hpp
	components/dice_loader.cpp
	components/dice.cpp
	components/dice.cpp components/dice_loader.cpp
	components/energy_meter.cpp
	components/game_piece_loader.cpp
	components/game_piece.cpp
	components/game_piece.cpp components/game_piece_loader.cpp
	components/health_meter.cpp
	components/pause.cpp
	components/player_loader.cpp
	components/player.cpp
	components/player.cpp components/player_loader.cpp
	components/ready_display_loader.cpp
	components/ready_display.cpp
	components/selection_arrow_loader.cpp
	components/selection_arrow_loader.cpp components/selection_arrow_loader.cpp
	components/selection_arrow.cpp
	components/selection_arrow.cpp components/selection_arrow.cpp
	components/terrain.cpp
	components/terrain.cpp
	components/tug_of_war_meter.cpp
	deck_selection_controller.cpp
	deck_ui_controller.cpp
	deck_ui_controller.cpp
	deck_ui_controller.hpp
	game_start_system.hpp
	main.cpp
	ready_display_system.cpp)

target_link_libraries(prototype_one PRIVATE
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
	engineui
	freetype)

add_custom_target(copy_prototype_one_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

add_dependencies (prototype_one copy_prototype_one_assets)

if (UNIX)
    target_link_libraries(prototype_one PRIVATE PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(prototype_one PRIVATE ${CMAKE_DL_LIBS})
endif ()

set_target_properties(prototype_one PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(prototype_one)
