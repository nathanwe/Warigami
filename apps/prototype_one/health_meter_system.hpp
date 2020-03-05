#ifndef WIZARD_GAME_HEALTH_METER_SYSTEM_HPP
#define WIZARD_GAME_HEALTH_METER_SYSTEM_HPP

#include "game_events.hpp"

#include "components/health_meter.hpp"

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <transforms/transform.hpp>

class health_meter_system : public ecs::system_base
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
		r_state.each<components::health_meter, transforms::transform>([&](auto& meter, auto& tform)
		{
			meter.first_position = tform.position;
		});
	}

	void not_first_run(ecs::state& r_state)
	{
		r_state.each<components::health_meter, transforms::transform>([&](auto& meter, auto& tform)
		{
			r_state.each<components::player>([&](auto& p)
			{
				if (p.team == meter.team)
				{
					tform.scale.x = p.health / 100.f * max_scale;
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