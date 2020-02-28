//
// Created by sava on 2/7/20.
//

#include <engine.hpp>

#include "fly_cam_system.hpp"

int main(int argc, char** argv)
{
	asset::asset_manager assets;
	core::startup_config config;
	core::glfw_context glfw(config);

	util::string_table strings;
	core::viewport window_view{ 0, 0, config.width(), config.height() };
	core::game_input_manager input(glfw.window());
	core::frame_timer timer;
	core::cursor_state cursor(glfw.window());
	event::EventManager events;

    // init ecs state
    ecs::archetype_pools memory;
    ecs::state state(memory);
    ecs::register_component<transforms::transform>("transform");
    ecs::register_component<rendering::camera>("camera");
    ecs::register_component<rendering::light_directional>("light_directional");
    ecs::register_component<rendering::light_point>("light_point");
    ecs::register_component<rendering::renderable_mesh_static>("renderable_mesh_static");
    ecs::register_component<rendering::renderable_mesh_rigged>("renderable_mesh_rigged");

	transforms::transformer transformer;
	rendering::asset_cache render_asset_cache(assets);
	rendering::camera_updater camera_updater;
	rendering::renderer renderer(glfw.window(), window_view, true, render_asset_cache, assets, timer);
	rendering::animator animator(timer);
	fly_cam flycam(input, timer, events);

	ecs::systems systems({
		&transformer,
		&camera_updater,
		&animator,
		&renderer,
		&flycam });

	ecs::world world(systems, state);

	asset::scene scene("assets/scene.json", assets);
	asset::scene_hydrater hydrater(state, scene);
	transforms::transform_loader transform_loader;
	rendering::loader_camera camera_loader(render_asset_cache);
	rendering::loader_light_directional dir_light_loader;
	rendering::loader_light_point point_light_loader;
	rendering::render_loader render_loader(render_asset_cache);
	rendering::loader_rigged_model rigged_model_loader(assets, render_asset_cache);

	hydrater.register_loaders(
		&camera_loader,
		&dir_light_loader,
		&point_light_loader,
		&render_loader,
		&transform_loader,
		&rigged_model_loader);

	hydrater.load();

	//game loop
	while (!glfwWindowShouldClose(glfw.window())) {
		timer.start();
		glfwPollEvents();

		input.update();
		world.update();

		glfw.swap_buffers();
		timer.end();
	}

	return 0;
}