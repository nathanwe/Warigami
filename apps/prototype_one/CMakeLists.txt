
add_executable (warigami
	board_update_system.hpp
	combat.cpp
	components/board_loader.cpp
	components/board_square_loader.cpp	
	components/board_square.cpp 	
	components/board.cpp	
	components/capture.cpp
	components/card_enum.hpp
	components/card_loader.cpp
	components/card.cpp	
	components/countdown.cpp
	components/deck_cursor.cpp
	components/deck_option.cpp
	components/deck_selection.cpp
	components/deck_ui.cpp	
	components/dice_loader.cpp
	components/dice.cpp	
	components/energy_ball.cpp
	components/energy_meter.cpp
	components/game_piece_loader.cpp
	components/game_piece.cpp	
	components/health_meter.cpp
	components/logo.cpp
	components/main_menu.cpp
	components/pause.cpp
	components/pause_arrow.cpp
	components/player_loader.cpp
	components/player.cpp	
	components/ready_display_loader.cpp
	components/ready_display.cpp
	components/selection_arrow_loader.cpp	
	components/selection_arrow.cpp
	components/spawn_effect.cpp
	components/terrain.cpp
	components/tug_of_war_meter.cpp
	components/tip.cpp
	components/tip_loader.cpp
	deck_selection_controller.cpp
	deck_ui_controller.cpp
	energy_ball_system.cpp
	game_start_system.hpp
	main.cpp
	ready_display_system.cpp
	main_menu_controller.cpp
	logo_system.cpp
	)

target_link_libraries(warigami PRIVATE
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

add_custom_target(copy_warigami_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

add_dependencies (warigami copy_warigami_assets)

if (UNIX)
    target_link_libraries(warigami PRIVATE PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(warigami PRIVATE ${CMAKE_DL_LIBS})
endif ()

set_target_properties(warigami PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)
cotire(warigami)
