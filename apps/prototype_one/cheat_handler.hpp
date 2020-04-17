#pragma once


#include <ecs/state.hpp>
#include <algorithm>
#include "components/board.hpp"
#include "components/pause.hpp"
#include "components/player.hpp"

#include <event/event.hpp>
#include <event/event_manager.hpp>


class cheat_handler : public event::Listener
{
public:
	cheat_handler(ecs::state& state, event::EventManager& _event_manager)
		: _state(state), event_manager(_event_manager) {
		event_manager.Subscribe(this, event::EVENT_TYPE::CHEAT);
	}

	void HandleEvent(event::Event& event)
	{
		using namespace std::string_literals;
		if (event.mType == event::EVENT_TYPE::CHEAT) {
			if (event.mData == "toggle_P1_AI"s)
			{
				_state.each<components::player>([&](components::player& player) {
					if (player.team == 1.0f) {
						player.controlled_by_AI = !player.controlled_by_AI;
					}
					});
			}
			if (event.mData == "toggle_P2_AI"s)
			{
				_state.each<components::player>([&](components::player& player) {
					if (player.team == -1.0f) {
						player.controlled_by_AI = !player.controlled_by_AI;
					}
					});
			}
			if (event.mData == "attract_mode"s)
			{
				_state.each<components::player>([&](components::player& player) {
					player.controlled_by_AI = true;
					});
				_state.each<components::board_square>([&](components::board_square& square) {
					if (square.y > 4) {
						square.next_team = 1.0;
					}
					else {
						square.next_team = -1.0;
					}
					});
			}
		}
	}

private:
	ecs::state& _state;
	event::EventManager& event_manager;
};
