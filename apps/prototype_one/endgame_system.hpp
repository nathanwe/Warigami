#ifndef WIZARD_GAME_ENDGAME_SYSTEM_HPP
#define WIZARD_GAME_ENDGAME_SYSTEM_HPP

#include "components/player.hpp"
#include "components/tug_of_war_meter.hpp"

#include <asset/scene_hydrater.hpp>
#include <ecs/system_base.hpp>

class endgame_system : public ecs::system_base
{
public:
	endgame_system(asset::scene_hydrater& hydrater) : _hydrater(hydrater)
	{
	}

public:
	void update(ecs::state& r_state) override
	{
		if (_did_game_end)
		{
			return;
		}

		prototype_one_condition(r_state);
		prototype_two_condition(r_state);
		check_endgame();
	}

	void prototype_one_condition(ecs::state& r_state)
	{
		if (_did_game_end)
		{
			return;
		}
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

	void prototype_two_condition(ecs::state& r_state)
	{
		if (_did_game_end)
		{
			return;
		}
		r_state.first<components::tug_of_war_meter>([&](auto& meter)
		{
			if (abs(meter.value) == 100.f)
			{
				_tie = _did_game_end == true;
				_did_game_end = true;
				_winner = meter.value / 100.f;
			}
			return true;
		});
	}

	void check_endgame()
	{
		if (_did_game_end)
		{
			if (_tie) { endgame_tie(); }
			else      { endgame_winlose(); }
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
	}

private:
	asset::scene_hydrater& _hydrater;
	bool _did_game_end = false;
	float _winner = 0.f;
	float _tie = false;
};

#endif
