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

// Game systems
#include "fly_cam_system.hpp"
#include "board_update_system.hpp"
#include "player_controller.hpp"
#include "util/boardgen.hpp"
#include "game_start_system.hpp"
#include "energy_meter_system.hpp"
#include "health_meter_system.hpp"
#include "tug_of_war_system.hpp"
#include "countdown_system.hpp"
#include "combat.hpp"



int main(int argc, char** argv) {

	// Absolute dank
	//boardgen::generateBoardJson("newboard.json", "assets/prototypes/boardsquare.json", "assets/prototypes/boardsquare2.json", 3, 4);

#ifndef NDEBUG
	bool is_debug = true;
#else
	bool is_debug = false;
#endif

	asset::asset_manager assets;
	core::startup_config config;
	core::glfw_context glfw(config);

	util::string_table strings;
	core::viewport window_view{ 0, 0, glfw.width(), glfw.height() };
	core::game_input_manager input(glfw.window());
	core::frame_timer timer;
	core::frame_limiter limiter(timer, 60);
	core::cursor_state cursor(glfw.window());
	event::EventManager events;
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

	asset::scene scene("assets/scenes/scene.json", assets);
	asset::scene_hydrater hydrater(state, scene);

	fly_cam flycam(input, timer, events);
	board_update board_updater(input, timer, events, hydrater, resolver);
	player_controller player_control(input, timer, events, hydrater);
	game_start_system game_start_system(hydrater);
	audio::audio_system audio_system(strings, assets);
	collisions::collision_manager collision_manager;
	physics::physics_update physics_update(collision_manager, timer);
	rendering::asset_cache render_asset_cache(assets);
	rendering::camera_updater camera_updater;
	rendering::render_state render_state;
	rendering::renderer renderer(window_view, is_debug, render_asset_cache, assets, timer, render_state);
	rendering::freetype_system text_renderer(window_view, assets, strings, render_state);
	transforms::transformer transformer;

	energy_meter_system energy_system;
	health_meter_system health_system;
	tug_of_war_meter_system tug_system;
	countdown_system count_system(timer, events, strings);

	ecs::systems systems({
		&energy_system,
		&health_system,
		&tug_system,
		&count_system,
		&transformer,
		&camera_updater,
		&renderer,
        &text_renderer,
		&flycam,
		&audio_system,
		&physics_update,
		&board_updater,
		&game_start_system,
		&player_control});

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
	collisions::aabb_collider_loader aabb_collider_loader;
	collisions::sphere_collider_loader sphere_collider_loader;
	collisions::rigid_body_loader rigid_body_loader;

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
		&text_loader);

	hydrater.load();

	engineui::developer_console console(window_view, events, glfw.window());
	engineui::fps_display fps(window_view, timer);
	engineui::entities_view entities_view(window_view, events, state);
	engineui::imgui_overlay overlay(glfw.window(), input, cursor);
	rendering::debug_view render_debug_view(window_view, renderer);
	overlay.register_views(&console, &fps, &entities_view, &render_debug_view);

	//cursor.disable();


	//game loop
	while (!glfwWindowShouldClose(glfw.window())) {
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
}
