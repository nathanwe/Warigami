#ifndef WIZARD_GAME_ENDGAME_SCREEN_SYSTEM_HPP
#define WIZARD_GAME_ENDGAME_SCREEN_SYSTEM_HPP

#include <core/frame_timer.hpp>
#include <core/game_input_manager.hpp>
#include <core/glfw_context.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <transforms/transform.hpp>

#include "components/pause.hpp"

class endgame_screen : public ecs::system_base
{
public:
	endgame_screen(core::game_input_manager& input, core::frame_timer& timer, core::glfw_context& glfw, event::EventManager& events)
		: m_r_input(input), m_r_timer(timer), m_r_glfw(glfw), m_r_events(events)
	{}

	void initialize(ecs::state& state) 
	{
		_current_warning_selection = 0;
		m_is_in_submenu = false;
	}

	void update(ecs::state& state)
	{
		auto p_pause_entity = state.first<components::pause>();
		auto p_menu_arrow_entity = state.first<components::pause_arrow>();
		if (p_pause_entity == nullptr || p_menu_arrow_entity == nullptr)
		{
			return;
		}
		auto& arrow_renderable = p_menu_arrow_entity->get_component<rendering::renderable_mesh_static>();
		const auto& pause = p_pause_entity->get_component<components::pause>();

		if (pause.is_game_over)
		{
			if (!m_is_in_submenu)
			{
				// Return to menu
				if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || 
					m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2))
				{
					auto camera_entity = state.first<rendering::camera>();
					auto& music = camera_entity->get_component<audio::music_player>();
					music.set_sound_state(0, audio::sound_state::stop_requested);

					asset::scene_change_event restart_event("assets/scenes/main_menu.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
				// Return to deck selection
				else if (m_r_input.is_input_started(core::controls::CARD2_CONTROL) || 
					m_r_input.is_input_started(core::controls::CARD2_CONTROL_PLAYER2))
				{
					auto camera_entity = state.first<rendering::camera>();
					auto& music = camera_entity->get_component<audio::music_player>();
					music.set_sound_state(0, audio::sound_state::stop_requested);

					asset::scene_change_event restart_event("assets/scenes/scene.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
				// Exit
				else if (m_r_input.is_input_started(core::controls::CARD3_CONTROL) || 
					m_r_input.is_input_started(core::controls::CARD3_CONTROL_PLAYER2))
				{
					m_is_in_submenu = true;
					_current_warning_selection = 0;
					arrow_renderable.is_enabled = true;
					return;
				}
			}
			else
			{
				auto& arrow_transform = p_menu_arrow_entity->get_component<transforms::transform>();
				auto& endscreen_renderable = pause.p_end_screen->get_component<rendering::renderable_mesh_static>();
				confirm_destructive_action(state, arrow_transform, arrow_renderable, endscreen_renderable);
			}
		}
	}

private:
	void confirm_destructive_action(ecs::state& state, transforms::transform& arrow_transform, rendering::renderable_mesh_static& arrow_renderable, rendering::renderable_mesh_static& pause_renderable) 
	{
		auto& e = state.find_entity(109); // Warning image
		auto& r = e.get_component<rendering::renderable_mesh_static>();
		r.is_enabled = true;
		pause_renderable.is_enabled = false;
		arrow_transform.position = glm::vec3(-27 - (_current_warning_selection + 2) * 0.7, 8 - (_current_warning_selection + 2) * 0.9, -3);
		arrow_transform.is_matrix_dirty = true;

		// Change selected
		if (m_r_input.is_input_started(core::controls::UP_CONTROL) || m_r_input.is_input_started(core::controls::UP_CONTROL_PLAYER2))
		{
			if (_current_warning_selection == 0) 
			{
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

			if (_current_warning_selection == 0) 
			{
				// Return to pause menu
				m_is_in_submenu = false;
				r.is_enabled = false;
				pause_renderable.is_enabled = true;
				arrow_transform.is_matrix_dirty = true;
				arrow_renderable.is_enabled = false;
			}
			else
			{
				m_r_glfw.set_should_close(true);
			}
		}
	}


private:
	core::frame_timer& m_r_timer;
	core::game_input_manager& m_r_input;
	core::glfw_context& m_r_glfw;
	event::EventManager& m_r_events;

	bool m_is_in_submenu = false;
	int _current_warning_selection;
};

#endif
