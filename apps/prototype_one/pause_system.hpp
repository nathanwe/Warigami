#ifndef WIZARD_GAME_PAUSE_SYSTEM_HPP
#define WIZARD_GAME_PAUSE_SYSTEM_HPP

#include <asset/scene_hydrater.hpp>
#include <core/frame_timer.hpp>
#include <core/game_input_manager.hpp>
#include <core/glfw_context.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <transforms/transform.hpp>
#include <asset/scene_change_event.hpp>
#include <event/event_manager.hpp>

#include "components/pause.hpp"

enum pause_options {
	RESUME,
	REPLAY,
	MAIN_MENU,
	QUIT,
	HOW_TO,
	OPTIONS,
	CREDITS,
	NUM_PAUSE_OPTIONS
};

class pause_system : public ecs::system_base
{
public:
	pause_system(
		core::game_input_manager& input,
		core::frame_timer& timer,
		core::glfw_context& glfw,
		asset::scene_hydrater& hydrater,
		event::EventManager& events)
		: m_r_input(input), m_r_timer(timer), m_r_glfw(glfw), m_r_hydrater(hydrater), m_r_events(events), _current_selection(0)
	{}

	void update(ecs::state& state) override
	{
		auto e = state.first<components::pause>(
			[&](auto& pause)
			{
				return true;
			});

		auto a = state.first<components::pause_arrow>([&](auto& pause_arrow)
		{
			return true;
		});

		if (e == nullptr || a == nullptr)
		{
			return;
		}

		auto& pause = e->get_component<components::pause>();

		if (!pause.is_game_started || !pause.is_game_countdown_over || pause.is_game_over)
		{
			return;
		}

		auto& renderable = e->get_component<rendering::renderable_mesh_static>();
		auto& arrow_renderable = a->get_component<rendering::renderable_mesh_static>();
		auto& arrow_transform = a->get_component<transforms::transform>();

		if (m_r_glfw.is_minimized())
		{
			pause.is_game_paused = true;
			renderable.is_enabled = true;
			arrow_renderable.is_enabled = true;
		}

		// Toggle pause
		if (m_r_input.is_input_started(core::controls::BACK_CONTROL))
		{
			pause.is_game_paused = !pause.is_game_paused;
			renderable.is_enabled = !renderable.is_enabled;
			arrow_renderable.is_enabled = !arrow_renderable.is_enabled;
		}

		if (pause.is_game_paused)
		{
			// Change selected
			if (m_r_input.is_input_started(core::controls::UP_CONTROL) || m_r_input.is_input_started(core::controls::UP_CONTROL_PLAYER2)) 
			{
				if (_current_selection == 0) {
					_current_selection = NUM_PAUSE_OPTIONS;
				}
				_current_selection--;
				arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
				arrow_transform.is_matrix_dirty = true;
			}
			else if (m_r_input.is_input_started(core::controls::DOWN_CONTROL) || m_r_input.is_input_started(core::controls::DOWN_CONTROL_PLAYER2))
			{
				_current_selection = (++_current_selection) % NUM_PAUSE_OPTIONS;
				arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
				arrow_transform.is_matrix_dirty = true;
			}

				// Choose the selected option
				else if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2)) 
			{
				// Note: I tried this with a switch statement but it ended up being messier because of bypassing initialization of variables
				if (_current_selection == RESUME) {
					// Resume
					pause.is_game_paused = false;
					renderable.is_enabled = false;

					arrow_renderable.is_enabled = false;
				}
				else if (_current_selection == REPLAY) {
					// Replay
					// TODO: Confirm destructive action
					pause.is_game_paused = false;
					renderable.is_enabled = false;
					arrow_renderable.is_enabled = false;
					asset::scene_change_event restart_event("assets/scenes/scene.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
				else if (_current_selection == MAIN_MENU) {
					// Main Menu
					pause.is_game_paused = false;
					renderable.is_enabled = false;
					arrow_renderable.is_enabled = false;
					asset::scene_change_event restart_event("assets/scenes/main_menu.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
				else if (_current_selection == QUIT) {
					// Quit
					// TODO: Confirm destructive action
					m_r_glfw.set_should_close(true);
				}
				else if (_current_selection == HOW_TO) {
					// How To
				}
				else if (_current_selection == OPTIONS) {
					// Options
				}
				else if (_current_selection == CREDITS) {
					// Credits
				}
			}
		}
	}

private:
	core::frame_timer& m_r_timer;
	core::game_input_manager& m_r_input;
	core::glfw_context& m_r_glfw;
	asset::scene_hydrater& m_r_hydrater;
	event::EventManager& m_r_events;
	int _current_selection;
};

#endif
