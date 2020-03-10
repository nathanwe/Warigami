#ifndef WIZARD_GAME_ENERGY_METER_SYSTEM_HPP
#define WIZARD_GAME_ENERGY_METER_SYSTEM_HPP

#include "components/energy_meter.hpp"
#include "components/player.hpp"

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <transforms/transform.hpp>

#include <algorithm>

class energy_meter_system : public ecs::system_base
{
public:
	void update(ecs::state& r_state) override
	{
		if (is_first_run)
		{
			first_run(r_state);
			is_first_run = false;
		}
		else
		{
			not_first_run(r_state);
		}
	}

	void first_run(ecs::state& r_state)
	{
		r_state.each<components::energy_meter, transforms::transform>([&](auto& meter, auto& tform)
		{
			meter.first_position = tform.position;
		});
	}

	void not_first_run(ecs::state& r_state)
	{
		r_state.each<components::energy_meter, transforms::transform>([&](auto& meter, auto& tform)
		{
			r_state.each<components::player>([&](auto& p)
			{
				if (p.team == meter.team)
				{
					float clamped = std::clamp(p.energy, 0, 10);
					tform.scale.x = clamped / 10.f * max_scale;
					tform.position.x = meter.first_position.x + tform.scale.x / 2.f;
					tform.is_matrix_dirty = true;
				}
			});
		});
	}

private:
	static constexpr float max_scale = 7.f;
	bool is_first_run = true;
};

#endif