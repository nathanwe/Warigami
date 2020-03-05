#ifndef WIZARD_GAME_TUG_OF_WAR_SYSTEM_HPP
#define WIZARD_GAME_TUG_OF_WAR_SYSTEM_HPP

#include "components/tug_of_war_meter.hpp"

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <transforms/transform.hpp>

#include <algorithm>

class tug_of_war_meter_system : public ecs::system_base
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
		r_state.each<components::tug_of_war_meter, transforms::transform>([&](auto& meter, auto& tform)
		{
			meter.first_position = tform.position;
		});
	}

	void not_first_run(ecs::state& r_state)
	{
		r_state.each<components::tug_of_war_meter, transforms::transform>([&](auto& meter, auto& tform)
		{
			float offset = std::clamp(meter.value / 200.f * max_tug, -max_tug, max_tug);
			
			tform.position.z = meter.first_position.z + offset;
			tform.is_matrix_dirty = true;
		});
	}

private:
	static constexpr float max_tug = 2.25f;
	bool is_first_run = true;
};

#endif