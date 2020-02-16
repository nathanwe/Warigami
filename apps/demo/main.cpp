// Engine systems
#include "asset/json_cache.hpp"
#include "asset/scene.hpp"
#include "asset/scene_hydrater.hpp"
#include "core/viewport.hpp"
#include <core/glfw_context.hpp>
#include "ecs/register_component.hpp"
#include "ecs/state.hpp"
#include "ecs/world.hpp"
#include "transforms/transform.hpp"
#include "transforms/transformer.hpp"
#include "transforms/transform_loader.hpp"
#include "rendering/camera.hpp"
#include "rendering/camera_updater.hpp"
#include "rendering/debug_view.hpp"
#include "rendering/light_directional.hpp"
#include "rendering/light_point.hpp"
#include "rendering/loader_camera.hpp"
#include "rendering/loader_light_directional.hpp"
#include "rendering/loader_light_point.hpp"
#include "rendering/render_loader.hpp"
#include "rendering/renderable_mesh_static.hpp"
#include "rendering/renderer.hpp"
#include "collisions/collider.hpp"
#include "collisions/aabb_collider_loader.hpp"
#include "collisions/sphere_collider_loader.hpp"
#include "collisions/rigid_body_loader.hpp"
#include "collisions/collision_manager.hpp"
#include <audio/audio_system.hpp>
#include <audio/loader_emitter.hpp>
#include <physics/physics_update.hpp>

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

// Game systems
#include "spinner.hpp"
#include "fly_cam_system.hpp"
#include "box_move_system.hpp"
#include "util/boardgen.hpp"

#include <engine-ui/imgui_overlay.hpp>
#include <engine-ui/developer_console.hpp>
#include <engine-ui/entities_view.hpp>
#include <engine-ui/fps_display.hpp>


int main(int argc, char** argv)
{
#ifndef NDEBUG
	bool is_debug = true;
#else
	bool is_debug = false;	
#endif

	core::startup_config config;
	core::glfw_context glfw(config);	

    util::string_table strings;
    core::viewport window_view{ 0, 0, config.width(), config.height() };
    core::game_input_manager input(glfw.window());
    core::frame_timer timer;
    core::cursor_state cursor(glfw.window());
    EventManager events;

    // init ecs state
    ecs::archetype_pools memory;
    ecs::state state(memory);
    ecs::register_component<components::game_piece>("game_piece");
    ecs::register_component<components::board>("board");
    ecs::register_component<components::board_square>("board_square");
    ecs::register_component<components::card>("card");
    ecs::register_component<components::dice>("dice");
    ecs::register_component<components::player>("player");
    ecs::register_component<transforms::transform>("transform");
    ecs::register_component<rendering::camera>("camera");
    ecs::register_component<rendering::light_directional>("light_directional");
    ecs::register_component<rendering::light_point>("light_point");
    ecs::register_component<rendering::renderable_mesh_static>("renderable_mesh_static");
    ecs::register_component<audio::audio_emitter>("audio_emitter");
    ecs::register_component<audio::audio_listener>("audio_listener");
    ecs::register_component<collisions::sphere_collider>("sphere_collider");
    ecs::register_component<collisions::AABB_collider>("aabb_collider");
    ecs::register_component<collisions::rigid_body>("rigid_body");

    collisions::collision_manager collision_manager;
    physics::physics_update physics_update(collision_manager, timer);
    rendering::asset_cache render_asset_cache;
    rendering::renderer renderer(glfw.window(), window_view, is_debug, render_asset_cache);
    transforms::transformer transformer;
    rendering::camera_updater camera_updater;
    audio::audio_system audio_system(strings);
    spinner spinner(timer);
    fly_cam flycam(input, timer);
    box_move boxmove(timer);
    ecs::systems systems({ &transformer, &camera_updater, &renderer, &flycam, &boxmove, &audio_system, &spinner, &physics_update });
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
    components::board_loader board_loader;
    components::board_square_loader board_square_loader;
    components::card_loader card_loader;
    components::dice_loader dice_loader;
    components::player_loader player_loader;
    collisions::aabb_collider_loader aabb_collider_loader;
    collisions::sphere_collider_loader sphere_collider_loader;
    collisions::rigid_body_loader rigid_body_loader;
    hydrater.register_loaders(&transform_loader, &camera_loader, &dir_light_loader, &point_light_loader, &render_loader, &eloader,
        &game_piece_loader, &board_loader, &board_square_loader, &card_loader, &dice_loader, &player_loader,
        &aabb_collider_loader, &sphere_collider_loader, &rigid_body_loader);
    hydrater.load();

    state.each<audio::audio_emitter>([](audio::audio_emitter& e) {
        e.set_sound_state(0, audio::playback_requested);
        });


    engineui::developer_console console(window_view, events);
    engineui::fps_display fps(window_view, timer);
    engineui::entities_view entities_view(window_view, events, state);
    engineui::imgui_overlay overlay(glfw.window(), input, cursor);
    rendering::debug_view render_debug_view(window_view, renderer);
    overlay.register_views(&console, &fps, &entities_view, &render_debug_view);

    cursor.disable();

    printf("Hello, I'm the dev console!");

    //game loop
    while (!glfwWindowShouldClose(glfw.window()))
    {
        timer.start();
        glfwPollEvents();

        input.update();
        world.update();

        entities_view.update(state);
        overlay.update();

        glfw.swap_buffers();
        timer.end();
    }
}
