#ifndef WIZARD_GAME_PAUSE_SYSTEM_HPP
#define WIZARD_GAME_PAUSE_SYSTEM_HPP

#include <core/frame_timer.hpp>
#include <core/game_input_manager.hpp>
#include <core/glfw_context.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <transforms/transform.hpp>

#include "components/pause.hpp"

class pause_system : public ecs::system_base
{
public:
	pause_system(core::game_input_manager& input, core::frame_timer& timer, core::glfw_context& glfw)
		: m_r_input(input), m_r_timer(timer), m_r_glfw(glfw)
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

			if (pause.is_game_paused)
			{
				// Resume
				if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2))
				{
					pause.is_game_paused = false;
					renderable.is_enabled = false;
				}
				// Return to menu
				else if (m_r_input.is_input_started(core::controls::CARD2_CONTROL) || m_r_input.is_input_started(core::controls::CARD2_CONTROL_PLAYER2))
				{
					pause.is_game_paused = false;
					renderable.is_enabled = false;

					// CONNECT WITH SAV ON MAIN MENU
				}
				// Exit
				else if (m_r_input.is_input_started(core::controls::CARD3_CONTROL) || m_r_input.is_input_started(core::controls::CARD3_CONTROL_PLAYER2))
				{
					m_r_glfw.set_should_close(true);
				}
			}
			else
			{
				// Pause
				if (m_r_input.is_input_started(core::controls::MENU_CONTROL))
				{
					pause.is_game_paused = true;
					renderable.is_enabled = true;
				}
			}
		}
	}

private:
	core::frame_timer& m_r_timer;
	core::game_input_manager& m_r_input;
	core::glfw_context& m_r_glfw;
};

#endif
