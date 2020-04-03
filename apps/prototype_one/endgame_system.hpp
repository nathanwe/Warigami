#ifndef WIZARD_GAME_ENDGAME_SYSTEM_HPP
#define WIZARD_GAME_ENDGAME_SYSTEM_HPP

#include "components/player.hpp"
#include "components/tug_of_war_meter.hpp"
#include "components/board_square.hpp"

#include <asset/scene_hydrater.hpp>
#include <ecs/system_base.hpp>
#include <event/event.hpp>
#include <event/event_manager.hpp>

#include <math.h>


class endgame_system : public ecs::system_base, event::Listener
{
public:
	endgame_system(ecs::state& state, event::EventManager& event_manager, asset::scene_hydrater& hydrater) :
		_state(state), _event_manager(event_manager), _hydrater(hydrater)
	{
	}

public:
	void initialize(ecs::state& r_state) override
	{
		_event_manager.Subscribe(this, event::EVENT_TYPE::GAME_OVER);
	}

	void update(ecs::state& r_state) override
	{
		if (!_did_game_end)
		{
			//tug_kill_condition(r_state);
			//health_kill_condition(r_state);
			terrain_kill_condition(r_state);
			check_endgame();
		}
	}

	void HandleEvent(event::Event& event) override
	{
	}

private:
	void health_kill_condition(ecs::state& r_state)
	{
		r_state.each<components::player>([&](auto& player)
		{
			if (player.health == 0.f)
			{
				if (!_did_game_end)
				{
					_winner = player.team * -1.f;
					_did_game_end = true;
				}
				else
				{
					_tie = true;
				}
			}
		});
	}
	void terrain_kill_condition(ecs::state& r_state)
	{		
		int player_1_square_total = 0;
		int player_minus_1_square_total = 0;
		r_state.each<components::board_square>([&](auto& square)
			{
				if (square.team == 1.0f) {
					player_1_square_total++;
				}
				if (square.team == -1.0f) {
					player_minus_1_square_total++;
				}
			});
		if (player_1_square_total >= squares_needed_to_win) {
			_did_game_end = true;
			_winner = 1.0f;
		}
		if (player_minus_1_square_total >= squares_needed_to_win) {
			_did_game_end = true;
			_winner = -1.0f;
		}
	}

	void tug_kill_condition(ecs::state& r_state)
	{
		r_state.first<components::tug_of_war_meter>([&](auto& meter)
		{
			if (abs(meter.value) == 100.f)
			{
				_tie = _did_game_end == true;
				_did_game_end = true;
				_winner = meter.value / -100.f;
			}
			return true;
		});
	}

	void health_timeout_condition(event::Event& event)
	{
		float highest_health = -1.f;
		float highest_team = 0.f;
		bool tied = false;
		_state.each<components::player>([&](auto& player)
		{
			if (player.health > highest_health)
			{
				highest_health = player.health;
				highest_team = player.team;
				tied = false;
			}
			else if (player.health == highest_health)
			{
				tied = true;
			}
		});
		_winner = highest_team;
		_tie = tied;
	}

	void tug_timeout_condition(event::Event& event)
	{
		bool found_meter = false;
		_state.first<components::tug_of_war_meter>([&](auto& meter)
		{
			_tie = meter.value == 0.f;
			_winner = signbit(meter.value) ? 1.f : -1.f;
			found_meter = true;
			return true;
		});
		assert(found_meter);
	}

	void check_endgame()
	{
		if (_did_game_end)
		{
			if (_tie) { endgame_tie(); }
			else { endgame_winlose(); }
		}
	}

	void endgame_tie()
	{
		_hydrater.add_from_prototype("assets/prototypes/endgame_tie_screen.json");
	}

	void endgame_winlose()
	{
		if (_winner > 0)
		{
			_hydrater.add_from_prototype("assets/prototypes/endgame_win_lose_screen.json");
		}
		else
		{
			_hydrater.add_from_prototype("assets/prototypes/endgame_lose_win_screen.json");
		}
		_state.each<components::pause>([&](auto& pause)
			{
				pause.is_game_over = true;
			});
	}

private:
	ecs::state& _state;
	event::EventManager& _event_manager;
	asset::scene_hydrater& _hydrater;
	bool _did_game_end = false;
	float _winner = 0.f;
	float _tie = false;
	static const int squares_needed_to_win = 45;
};

#endif
