#include <engine.hpp>
#include <rendering/freetype_system.hpp>
#include <rendering/loader_renderable_text.hpp>
#include <rendering/renderable_text.hpp>
#include "core/frame_limiter.hpp"

// Components
#include "components/game_piece.hpp"
#include "components/game_piece_loader.hpp"
#include "components/board.hpp"
#include "components/board_loader.hpp"
#include "components/board_square.hpp"
#include "components/board_square_loader.hpp"
#include "components/card.hpp"
#include "components/card_loader.hpp"
#include "components/dice.hpp"
#include "components/dice_loader.hpp"
#include "components/player.hpp"
#include "components/player_loader.hpp"
#include "components/energy_meter.hpp"
#include "components/energy_meter_loader.hpp"
#include "components/health_meter.hpp"
#include "components/health_meter_loader.hpp"
#include "components/tug_of_war_meter.hpp"
#include "components/tug_of_war_meter_loader.hpp"
#include "components/countdown.hpp"
#include "components/countdown_loader.hpp"
#include "components/terrain.hpp"
#include "components/deck_option.hpp"
#include "components/deck_selection.hpp"
#include "components/deck_selection_loader.hpp"
#include "components/deck_option_loader.hpp"
#include "components/deck_ui_loader.hpp"
#include "components/selection_arrow.hpp"
#include "components/selection_arrow_loader.hpp"
#include "components/deck_option.hpp"
#include "components/deck_selection.hpp"
#include "components/deck_selection_loader.hpp"
#include "components/deck_option_loader.hpp"
#include "components/deck_ui_loader.hpp"
#include "components/ready_display.hpp"
#include "components/ready_display_loader.hpp"
#include "components/main_menu.hpp"
#include "components/energy_ball.hpp"
#include "components/logo.hpp"
#include "components/pause_arrow.hpp"
#include "components/deck_cursor.hpp"
#include "components/deck_cursor_loader.hpp"

// Game systems
#include "fly_cam_system.hpp"
#include "board_update_system.hpp"
#include "player_controller.hpp"
#include "game_start_system.hpp"
#include "energy_meter_system.hpp"
#include "health_meter_system.hpp"
#include "tug_of_war_system.hpp"
#include "countdown_system.hpp"
#include "combat.hpp"
#include "components/deck_ui.hpp"
#include "deck_ui_controller.hpp"
#include "endgame_system.hpp"
#include "tick_update_system.hpp"
#include "spiderling_system.hpp"
#include "spawner_system.hpp"
#include "deck_selection_controller.hpp"
#include "pause_system.hpp"
#include "animator_system.hpp"
#include "terrain_update_system.hpp"
#include "health_regenration_system.hpp"
#include "AI_system.hpp"
#include "flash_step_system.hpp"
#include "ready_display_system.hpp"
#include "endgame_screen_system.hpp"
#include "energy_flower_creation_system.hpp"
#include "main_menu_controller.hpp"
#include "energy_ball_system.hpp"
#include "territory_claim_system.hpp"
#include "logo_system.hpp"


int main(int argc, char** argv) {	
#ifndef NDEBUG
	bool is_debug = true;
#else
	bool is_debug = false;
#endif

	util::string_table strings;
	event::EventManager events;
	asset::scene_tracker scene_tracker("assets/scenes/logo.json", events);
	asset::asset_manager assets;
	core::startup_config config;
	core::glfw_context glfw(config);
	core::viewport window_view{ 0, 0, glfw.width(), glfw.height() };
	core::game_input_manager input(glfw.window());
	core::frame_timer timer;
	core::frame_limiter limiter(timer, 60);
	core::cursor_state cursor(glfw.window());	
	
	combats::combat_resolution resolver;

	// init ecs state
	ecs::archetype_pools memory;
	ecs::state state(memory);
	ecs::register_component<components::game_piece>("game_piece");
	ecs::register_component<components::board>("board");
	ecs::register_component<components::board_square>("board_square");
	ecs::register_component<components::card>("card");
	ecs::register_component<components::dice>("dice");
	ecs::register_component<components::player>("player");
	ecs::register_component<components::energy_meter>("energy_meter");
	ecs::register_component<components::health_meter>("health_meter");
	ecs::register_component<components::tug_of_war_meter>("tug_of_war_meter");
	ecs::register_component<components::countdown>("countdown");
	ecs::register_component<components::selection_arrow>("selection_arrow");
	ecs::register_component<components::pause>("pause");
	ecs::register_component<components::pause_arrow>("pause_arrow");
	ecs::register_component<transforms::transform>("transform");
    ecs::register_component<components::deck_ui>("deck_ui");
	ecs::register_component<components::terrain>("terrain");
	ecs::register_component<components::deck_cursor>("deck_cursor");
	ecs::register_component<components::deck_option>("deck_option");
	ecs::register_component<components::deck_selection>("deck_selection");
	ecs::register_component<components::ready_display>("ready_display");
	ecs::register_component<components::main_menu>("main_menu");
	ecs::register_component<components::energy_ball>("energy_ball");
	ecs::register_component<components::logo>("logo");	
	ecs::register_component<transforms::transform>("transform");
	ecs::register_component<rendering::camera>("camera");
	ecs::register_component<rendering::light_directional>("light_directional");
	ecs::register_component<rendering::light_point>("light_point");
	ecs::register_component<rendering::renderable_mesh_static>("renderable_mesh_static");
	ecs::register_component<rendering::renderable_model_static>("model");
    ecs::register_component<rendering::renderable_text>("renderable_text");
	ecs::register_component<audio::audio_emitter>("audio_emitter");
	ecs::register_component<audio::audio_listener>("audio_listener");
	ecs::register_component<collisions::sphere_collider>("sphere_collider");
	ecs::register_component<collisions::AABB_collider>("aabb_collider");
	ecs::register_component<collisions::rigid_body>("rigid_body");

	asset::scene_hydrater hydrater(state);

	card_spawner card_spawn_helper(hydrater);

	fly_cam flycam(input, timer, events);
	board_update board_updater(input, timer, events, hydrater, resolver);
	player_controller player_control(input, timer, events, hydrater);
	
	deck_ui_controller deck_ui_controller(card_spawn_helper);
	audio::audio_system audio_system(strings, assets, config);
	collisions::collision_manager collision_manager;
	physics::physics_update physics_update(collision_manager, timer);
	rendering::asset_cache render_asset_cache(assets);
	game_start_system game_start_system(hydrater, render_asset_cache);
	health_regenration_system health_regen;
	
	rendering::camera_updater camera_updater;
	rendering::render_state render_state;
	rendering::renderer renderer(window_view, is_debug, render_asset_cache, assets, timer, render_state, glfw);
	rendering::freetype_system text_renderer(window_view, assets, strings, render_state);
	transforms::transformer transformer(hydrater);

	energy_meter_system energy_system(strings, glfw);
	health_meter_system health_system;
	tug_of_war_meter_system tug_system;
	countdown_system count_system(timer, events, strings, glfw);
	endgame_system endgame(state, events, hydrater);
	tick_update_system ticker(timer);
	spiderling_system spiderlings(hydrater);
	spawner_system spawner(hydrater);
	deck_selection_controller deck_selection(hydrater, card_spawn_helper, input, timer);
	pause_system pauser(input, timer, glfw, hydrater, events, audio_system);
	animator_system animator(timer);
	terrain_update_system terrain_update_system(timer, render_asset_cache);
	AI_system AI_system;
	flash_step_system flash_step_system;
	ready_display_system ready_display(glfw);
	endgame_screen escreen(input, timer, glfw, events);
	energy_flower_creation_system energy_flower_creation_system;
	main_menu_controller main_menu_controller(events, glfw, input, timer);
	energy_ball_system energy_balls(input, hydrater, timer);
	territory_claim_system territory_claim_system;
	logo_system logo(timer, events, input);

	ecs::systems systems({
		&energy_system,
		&health_system,
		&tug_system,
		&count_system,
		&camera_updater,
		&flycam,
		&audio_system,
		&physics_update,
		
		// order of these matters
		&game_start_system,		
		&ticker,			
		&energy_flower_creation_system,
		&terrain_update_system,
		&territory_claim_system,
		&health_regen,
		&spiderlings,	
		&board_updater,
		&flash_step_system,
		&AI_system,		
		&player_control,	
		&deck_selection,
		&spawner,

		&deck_ui_controller,
		&ready_display,
		&pauser,
		&endgame,
		&escreen,

		//
		&animator,
		&transformer,
		&renderer,
		&text_renderer,
		&main_menu_controller,
		&energy_balls,
		&logo
	});

	ecs::world world(systems, state);
	audio::loader_emitter eloader(strings);
	transforms::transform_loader transform_loader;
	rendering::loader_camera camera_loader(render_asset_cache);
	rendering::loader_light_directional dir_light_loader;
	rendering::loader_light_point point_light_loader;
	rendering::loader_model model_loader(render_asset_cache);
	rendering::render_loader render_loader(render_asset_cache);
	rendering::loader_renderable_text text_loader(strings);
	components::game_piece_loader game_piece_loader;
	components::board_loader board_loader;
	components::board_square_loader board_square_loader;
	components::card_loader card_loader;
	components::dice_loader dice_loader;
	components::player_loader player_loader;
	components::energy_meter_loader energy_loader;
	components::health_meter_loader health_loader;
	components::tug_of_war_meter_loader tug_loader;
	components::countdown_loader countdown_loader;
	components::selection_arrow_loader selection_arrow_loader;
	components::deck_selection_loader deck_selection_loader;
	components::deck_option_loader deck_option_loader;
	components::deck_ui_loader deck_ui_loader;	
	collisions::aabb_collider_loader aabb_collider_loader;
	collisions::sphere_collider_loader sphere_collider_loader;
	collisions::rigid_body_loader rigid_body_loader;
	components::ready_display_loader ready_display_loader;
	components::deck_cursor_loader deck_cursor_loader;

	hydrater.register_loaders(
		&aabb_collider_loader,
		&board_loader,
		&board_square_loader,
		&camera_loader,
		&card_loader,
		&dice_loader,
		&dir_light_loader,
		&eloader,
		&game_piece_loader,
		&player_loader,
		&point_light_loader,
		&render_loader,
		&rigid_body_loader,
		&sphere_collider_loader,
		&transform_loader,
		&model_loader,
		&energy_loader,
		&health_loader,
		&tug_loader,
		&countdown_loader,
		&selection_arrow_loader,
		&text_loader,
		&deck_selection_loader,
		&deck_option_loader,
		&deck_ui_loader,
		&ready_display_loader,
		&deck_cursor_loader);

	engineui::developer_console console(window_view, events, glfw.window());
	engineui::fps_display fps(window_view, timer);
	engineui::entities_view entities_view(window_view, events, state);
	engineui::imgui_overlay overlay(glfw.window(), input, cursor);
	rendering::debug_view render_debug_view(window_view, renderer);
	overlay.register_views(&console, &fps, &entities_view, &render_debug_view);

	while (scene_tracker.has_next() && !glfwWindowShouldClose(glfw.window()))
	{		
		asset::scene scene(scene_tracker.next(), assets);
		hydrater.populate_entities(scene);
		hydrater.load();
		world.initialize();

		//game loop
		while (!scene_tracker.has_next() && !glfwWindowShouldClose(glfw.window())) {
			timer.start();
			glfwPollEvents();

			input.update();
			world.update();

			entities_view.update(state);
			overlay.update();

			glfw.swap_buffers();
			hydrater.flush_removed();

			limiter.wait_remainder();
			timer.end();
		}

		hydrater.clear();
		state.free_all();
	}

	return 0;
}
