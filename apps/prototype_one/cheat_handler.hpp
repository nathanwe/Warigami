#pragma once


#include <ecs/state.hpp>
#include <algorithm>
#include "components/board.hpp"
#include "components/pause.hpp"
#include "components/player.hpp"

#include <event/event.hpp>


class cheat_handler : public event::Listener
{
public:
	cheat_handler(ecs::state& state)
		: _state(state) {}

	void HandleEvent(event::Event& event)
	{
		if (event.mType == event::EVENT_TYPE::TOGGLE_P1_AI)
		{
			_state.each<components::player>([&](components::player) {
				// ...
			});
		}
	}

private:
	ecs::state& _state;

};
