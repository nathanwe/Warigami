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
		: m_r_input(input), m_r_timer(timer), m_r_glfw(glfw), m_r_hydrater(hydrater), m_r_events(events), _current_selection(0),
		_seeing_message(false), how_to_page(0), _current_options_selection(0), _current_warning_selection(0)
	{}

	void initialize(ecs::state& state) {
		_current_selection = 0;

		_seeing_message = false;
		how_to_page = 0;
		_current_options_selection = 0;
		_current_warning_selection = 0;
	}

	void update(ecs::state& state) override
	{
		auto e = state.first<components::pause>();
		auto a = state.first<components::pause_arrow>();

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

		// Handle the popup screens instead of the pause menu
		if (_seeing_message) {
			if (_current_selection == HOW_TO) {
				handle_howto_case(state);
			}
			else if (_current_selection == OPTIONS) {
				handle_options_case(state, arrow_transform, renderable);
			}
			else if (_current_selection == MAIN_MENU || _current_selection == REPLAY || _current_selection == QUIT) {
				confirm_destructive_action(state, arrow_transform, arrow_renderable, renderable, pause);
			}
			return;
		}

		if (m_r_glfw.is_minimized())
		{
			pause.is_game_paused = true;
			renderable.is_enabled = true;
			arrow_renderable.is_enabled = true;
		}

		// Toggle pause
		if (m_r_input.is_input_started(core::controls::MENU_CONTROL))
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
					_seeing_message = true;
				}
				else if (_current_selection == MAIN_MENU) {
					// Main Menu
					_seeing_message = true;
				}
				else if (_current_selection == QUIT) {
					// Quit
					_seeing_message = true;
				}
				else if (_current_selection == HOW_TO) {
					// How To
					_seeing_message = true;
				}
				else if (_current_selection == OPTIONS) {
					// Options
					_seeing_message = true;
					arrow_transform.position = glm::vec3(-27, 8, -5);
					arrow_transform.is_matrix_dirty = true;
				}
				else if (_current_selection == CREDITS) {
					// Credits
				}
			}
		}
	}

	void handle_howto_case(ecs::state& state)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			auto& e = state.find_entity(how_to_play_images[i]);
			auto& r = e.get_component<rendering::renderable_mesh_static>();
			if (i == how_to_page) {
				r.is_enabled = true;
			}
			else {
				r.is_enabled = false;
			}
		}

		if (m_r_input.any_button_pressed())
		{
			how_to_page++;
			if (how_to_page > 3) {
				auto& e = state.find_entity(how_to_play_images[3]);
				auto& r = e.get_component<rendering::renderable_mesh_static>();
				r.is_enabled = false;
				how_to_page = 0;
				_seeing_message = false;
			}
		}
	}

	enum options_choices {
		FULLSCREEN,
		MUTE_ALL,
		MUTE_MUSIC,
		BACK,
		NUM_OPTIONS
	};

	void handle_options_case(ecs::state& state, transforms::transform& arrow_transform, rendering::renderable_mesh_static& pause_renderable) {
		auto& e = state.find_entity(108); // Options image
		auto& r = e.get_component<rendering::renderable_mesh_static>();
		r.is_enabled = true;
		pause_renderable.is_enabled = false;

		// Change selected
		if (m_r_input.is_input_started(core::controls::UP_CONTROL) || m_r_input.is_input_started(core::controls::UP_CONTROL_PLAYER2))
		{
			if (_current_options_selection == 0) {
				_current_options_selection = NUM_OPTIONS;
			}
			_current_options_selection--;
			arrow_transform.position = glm::vec3(-27 - _current_options_selection * 0.7, 8 - _current_options_selection * 0.9, -5);
			arrow_transform.is_matrix_dirty = true;
		}
		else if (m_r_input.is_input_started(core::controls::DOWN_CONTROL) || m_r_input.is_input_started(core::controls::DOWN_CONTROL_PLAYER2))
		{
			_current_options_selection = (++_current_options_selection) % NUM_OPTIONS;
			arrow_transform.position = glm::vec3(-27 - _current_options_selection * 0.7, 8 - _current_options_selection * 0.9, -5);
			arrow_transform.is_matrix_dirty = true;
		}
		// Choose selected
		else if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2)) {
			
			if (_current_options_selection == FULLSCREEN) {
				// Fullscreen
			}
			else if (_current_options_selection == MUTE_ALL) {
				// Mute all
			}
			else if (_current_options_selection == MUTE_MUSIC) {
				// Mute music
			}
			else if (_current_options_selection == BACK) {
				_seeing_message = false;
				r.is_enabled = false;
				pause_renderable.is_enabled = true;
				arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
				arrow_transform.is_matrix_dirty = true;
			}
		}
	}

	void confirm_destructive_action(ecs::state& state, transforms::transform& arrow_transform, rendering::renderable_mesh_static& arrow_renderable, rendering::renderable_mesh_static& pause_renderable, components::pause pause) {
		auto& e = state.find_entity(109); // Warning image
		auto& r = e.get_component<rendering::renderable_mesh_static>();
		r.is_enabled = true;
		pause_renderable.is_enabled = false;
		arrow_transform.position = glm::vec3(-27 - (_current_warning_selection + 2) * 0.7, 8 - (_current_warning_selection + 2) * 0.9, -3);
		arrow_transform.is_matrix_dirty = true;

		// Change selected
		if (m_r_input.is_input_started(core::controls::UP_CONTROL) || m_r_input.is_input_started(core::controls::UP_CONTROL_PLAYER2))
		{
			if (_current_warning_selection == 0) {
				_current_warning_selection = 2;
			}
			_current_warning_selection--;
		}
		else if (m_r_input.is_input_started(core::controls::DOWN_CONTROL) || m_r_input.is_input_started(core::controls::DOWN_CONTROL_PLAYER2))
		{
			_current_warning_selection = (++_current_warning_selection) % 2;
		}
		// Choose selected
		else if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2)) {

			if (_current_warning_selection == 0) {
				// Return to pause menu
				_seeing_message = false;
				r.is_enabled = false;
				pause_renderable.is_enabled = true;
				arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
				arrow_transform.is_matrix_dirty = true;
			}
			else if (_current_warning_selection == 1) {
				// Confirm action
				if (_current_selection == QUIT) {
					m_r_glfw.set_should_close(true);
				}
				else if (_current_selection == MAIN_MENU) {
					pause.is_game_paused = false;
					pause_renderable.is_enabled = false;
					arrow_renderable.is_enabled = false;
					asset::scene_change_event restart_event("assets/scenes/main_menu.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
				else if (_current_selection == REPLAY) {
					pause.is_game_paused = false;
					pause_renderable.is_enabled = false;
					arrow_renderable.is_enabled = false;
					asset::scene_change_event restart_event("assets/scenes/scene.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
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

	bool _seeing_message;
	int how_to_page;
	entity_id how_to_play_images[4]{ 104, 105, 106, 107 };
	int _current_options_selection;
	int _current_warning_selection;
};

#endif
