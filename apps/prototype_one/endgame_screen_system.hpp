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

	void update(ecs::state& state)
	{
		auto e = state.first<components::pause>([&](auto& pause)
		{
			return true;
		});
		if (e != nullptr)
		{
			auto& pause = e->get_component<components::pause>();
			auto& renderable = e->get_component<rendering::renderable_mesh_static>();

			if (pause.is_game_over)
			{
				// Return to menu
				if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2))
				{

					asset::scene_change_event restart_event("assets/scenes/scene.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
				// Return to deck selection
				else if (m_r_input.is_input_started(core::controls::CARD2_CONTROL) || m_r_input.is_input_started(core::controls::CARD2_CONTROL_PLAYER2))
				{

					asset::scene_change_event restart_event("assets/scenes/scene.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
				// Exit
				else if (m_r_input.is_input_started(core::controls::CARD3_CONTROL) || m_r_input.is_input_started(core::controls::CARD3_CONTROL_PLAYER2))
				{
					m_r_glfw.set_should_close(true);
					return;
				}
			}
		}
	}

private:
	core::frame_timer& m_r_timer;
	core::game_input_manager& m_r_input;
	core::glfw_context& m_r_glfw;
	event::EventManager& m_r_events;
};

#endif
