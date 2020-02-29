#ifndef GAME_PLAYER_CONTROLLER_HPP
#define GAME_PLAYER_CONTROLLER_HPP

#include "ecs/state.hpp"
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <event/event_manager.hpp>
namespace game {
	enum class PLAYER_STATE
	{
		BASE,
		PLACEMENT
	};
}
class player_controller : public ecs::system_base, public event::Listener
{
public:

	player_controller(	core::game_input_manager& input, core::frame_timer& timer, 
						event::EventManager& _event_manager, asset::scene_hydrater& _hydrater) : 
						m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater)
	{

	}

	void HandleEvent(event::Event& event)
	{

	}

	virtual void update(ecs::state& r_state) override
	{
		if (state == game::PLAYER_STATE::BASE)
		{
			if (m_input.is_input_active(core::ACTION1_CONTROL))
			{
				// Select first card
				state = game::PLAYER_STATE::PLACEMENT;
			}
			else if (m_input.is_input_active(core::ACTION2_CONTROL))
			{
				// Select second card
				state = game::PLAYER_STATE::PLACEMENT;
			}
			else if (m_input.is_input_active(core::ACTION3_CONTROL))
			{
				// Select third card
				state = game::PLAYER_STATE::PLACEMENT;
			}
			else if (m_input.is_input_active(core::ACTION4_CONTROL))
			{
				// Select fourth card
				state = game::PLAYER_STATE::PLACEMENT;
			}
		}
		else if (state == game::PLAYER_STATE::PLACEMENT)
		{
			if (m_input.is_input_active(core::ACTION1_CONTROL))
			{
				// Place unit
				state = game::PLAYER_STATE::BASE;
			}
			else if (m_input.is_input_active(core::ACTION2_CONTROL))
			{
				// Cancel placement
				state = game::PLAYER_STATE::BASE;
			}
			else if (m_input.forward() > .4f) 
			{
				// Select down
			}
			else if (m_input.forward() < .4f)
			{
				// Select down
			}
		}
	}

private:
	game::PLAYER_STATE state = game::PLAYER_STATE::BASE;
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
	asset::scene_hydrater& hydrater;
};

#endif