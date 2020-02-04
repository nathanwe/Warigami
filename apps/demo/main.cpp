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
#include "core/game_input_manager.hpp"
#include "components/game_piece.hpp"
#include "components/game_piece_loader.hpp"

#include "fly_cam_system.hpp"
#include <audio/audio_system.hpp>
#include <audio/loader_emitter.hpp>

void run_game(GLFWwindow* window, uint32_t window_width, uint32_t window_height, bool is_debug);





class spinner : public ecs::system_base
{
public:
	spinner(core::frame_timer& timer) :
		_timer(timer)
	{
	}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each< transforms::transform, rendering::renderable_mesh_static>([&](auto& transform, auto& renderable)
			{
				transform.rotation.y += m_rotation_speed * _timer.delta_secs();
				transform.is_matrix_dirty = true;
			});
	}

private:
	core::frame_timer& _timer;
	float m_rotation_speed = .5f;
};




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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
    util::string_table strings;

    rendering::viewport window_view{ 0, 0, window_width, window_height };

    core::game_input_manager input(window);
    core::frame_timer timer;

    // init ecs state
    ecs::archetype_pools memory;
    ecs::state state(memory);
	ecs::register_component<components::game_piece>("game_piece");
    ecs::register_component<transforms::transform>("transform");
    ecs::register_component<rendering::camera>("camera");
    ecs::register_component<rendering::light_directional>("light_directional");
    ecs::register_component<rendering::light_point>("light_point");
    ecs::register_component<rendering::renderable_mesh_static>("renderable_mesh_static");
    ecs::register_component<audio::audio_emitter>("audio_emitter");
    ecs::register_component<audio::audio_listener>("audio_listener");

    rendering::asset_cache render_asset_cache;
    rendering::renderer renderer(window, window_view, is_debug, render_asset_cache);
    transforms::transformer transformer;
    rendering::camera_updater camera_updater;
    audio::audio_system audio_system(strings);
	spinner spinner(timer);
    fly_cam flycam(input, timer);
    ecs::systems systems({ &transformer, &camera_updater, &renderer, &flycam, &audio_system, &spinner });
    ecs::world world(systems, state);
    
    audio::loader_emitter eloader(strings);

    asset::json_cache cache;
    asset::scene scene("assets/scenes/scene.json", cache);
    asset::scene_hydrater hydrater(state, scene);
    transforms::transform_loader transform_loader;
    rendering::loader_camera camera_loader(render_asset_cache);
    rendering::loader_light_directional dir_light_loader;
    rendering::loader_light_point point_light_loader;
    rendering::render_loader render_loader(render_asset_cache);
	components::game_piece_loader game_piece_loader;
    hydrater.register_loaders(&transform_loader, &camera_loader, &dir_light_loader, &point_light_loader, &render_loader, &eloader, &game_piece_loader);
    hydrater.load();

    state.each<audio::audio_emitter>([](audio::audio_emitter& e) {
        e.set_sound_state(0, audio::playback_requested);
    });

    //game loop
    while (!glfwWindowShouldClose(window))
    {
        timer.start();
        glfwPollEvents();
        input.update();
        world.update();
        timer.end();
    }
}

