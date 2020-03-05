#ifndef WIZARD_GAME_TUG_OF_WAR_SYSTEM_HPP
#define WIZARD_GAME_TUG_OF_WAR_SYSTEM_HPP

#include "game_events.hpp"

#include "components/tug_of_war_meter.hpp"

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <transforms/transform.hpp>

class tug_of_war_meter_system : public ecs::system_base
{
public:
	void update(ecs::state& r_state) override
	{

	}

private:

};

#endif