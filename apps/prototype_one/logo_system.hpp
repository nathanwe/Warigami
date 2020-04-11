#ifndef __LOGO_SYSTEM_HPP_
#define __LOGO_SYSTEM_HPP_

#include <ecs/state.hpp>
#include <ecs/system_base.hpp>
#include <core/frame_timer.hpp>
#include <event/event_manager.hpp>

class logo_system : public ecs::system_base
{
public:
	logo_system(
		core::frame_timer& timer,
		event::EventManager& events);
	
	void update(ecs::state& state) override;

private:
	core::frame_timer& _timer;
	event::EventManager& _events;
};


#endif