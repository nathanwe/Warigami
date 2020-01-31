#include "asset/json_cache.hpp"
#include "asset/scene.hpp"
#include "asset/scene_hydrater.hpp"
#include "ecs/register_component.hpp"
#include "ecs/state.hpp"
#include "ecs/world.hpp"
#include "transforms/transform.hpp"
#include "transforms/transformer.hpp"
#include "transforms/transform_loader.hpp"
#include "rendering/camera.hpp"
#include "rendering/camera_updater.hpp"
#include "rendering/light_directional.hpp"
#include "rendering/light_point.hpp"
#include "rendering/loader_camera.hpp"
#include "rendering/loader_light_directional.hpp"
#include "rendering/loader_light_point.hpp"
#include "rendering/render_loader.hpp"
#include "rendering/renderable_mesh_static.hpp"
#include "rendering/renderer.hpp"

#include "fly_cam_system.hpp"

void run_game(GLFWwindow* window, uint32_t window_width, uint32_t window_height, bool is_debug);

int main(int argc, char** argv)
{
#ifndef NDEBUG
	bool is_debug = true;
#else
	bool is_debug = false;
#endif

	// init glfw
	if (!glfwInit())
	{
		std::cout << "Error: GLFW failed to initialize." << std::endl;
		return -1;
	}

	uint32_t window_width = 1280;
	uint32_t window_height = 720;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RED_BITS, 32);
	glfwWindowHint(GLFW_GREEN_BITS, 32);
	glfwWindowHint(GLFW_BLUE_BITS, 32);
	glfwWindowHint(GLFW_ALPHA_BITS, 32);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	if (is_debug)
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	}
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Wizard People", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Error: GLFW failed to create window." << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

    run_game(window, window_width, window_height, is_debug);

	glfwTerminate();
}

void run_game(GLFWwindow* window, uint32_t window_width, uint32_t window_height, bool is_debug)
{
    rendering::viewport window_view{ 0, 0, window_width, window_height };

    core::input_manager input(window);
    core::frame_timer timer;

    // init ecs state
    ecs::archetype_pools memory;
    ecs::state state(memory);
    ecs::register_component<transforms::transform>("transform");
    ecs::register_component<rendering::camera>("camera");
    ecs::register_component<rendering::light_directional>("light_directional");
    ecs::register_component<rendering::light_point>("light_point");
    ecs::register_component<rendering::renderable_mesh_static>("renderable_mesh_static");
    rendering::asset_cache render_asset_cache;

    rendering::renderer renderer(window, window_view, is_debug);
    transforms::transformer transformer;
    rendering::camera_updater camera_updater;
    fly_cam flycam(input, timer);
    ecs::systems systems({ &transformer, &camera_updater, &renderer, &flycam });
    ecs::world world(systems, state);

    asset::json_cache cache;
    asset::scene scene("assets/scenes/scene.json", cache);
    asset::scene_hydrater hydrater(state, scene);
    transforms::transform_loader transform_loader;
    rendering::loader_camera camera_loader(render_asset_cache);
    rendering::loader_light_directional dir_light_loader;
    rendering::loader_light_point point_light_loader;
    rendering::render_loader render_loader(render_asset_cache);
    hydrater.register_loaders(&transform_loader, &camera_loader, &dir_light_loader, &point_light_loader, &render_loader);
    hydrater.load();

    //game loop
    while (!glfwWindowShouldClose(window))
    {
        timer.start();
        glfwPollEvents();
        input.update();
        if (input.was_key_pressed(GLFW_KEY_ESCAPE)) {
            glfwWindowShouldClose(window);
        }
        world.update();
        timer.end();
    }
}