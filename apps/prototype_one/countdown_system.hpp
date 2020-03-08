#ifndef WIZARD_GAME_COUNTDOWN_SYSTEM_HPP
#define WIZARD_GAME_COUNTDOWN_SYSTEM_HPP

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/frame_timer.hpp>
#include <util/string_table.hpp>

#include "components/countdown.hpp"

class game_over_event : public event::Event
{
public:
	explicit game_over_event() : event::Event(event::EVENT_TYPE::GAME_OVER)
	{}
};

class countdown_system : public ecs::system_base
{
public:
	countdown_system(
		core::frame_timer& timer, 
		event::EventManager& _event_manager,
		util::string_table& _strings)
		: m_timer(timer),
		event_manager(_event_manager),
		_strings(_strings) {}

	void update(ecs::state& r_state) override
	{
		if (is_first_run)
		{
			r_state.each<components::countdown, rendering::renderable_text>([&](auto& countdown, rendering::renderable_text& text)
			{
				countdown.current_value = countdown.count_duration;
				text.position = glm::ivec2(config.width()/2 - 50, config.height() - 200);
				text.scale = 2.0;
				text.string_hash = _strings.hash_and_store(std::to_string((int)countdown.current_value));
			});
			is_first_run = false;
		}
		else if (!ended)
		{
			r_state.each<components::countdown, rendering::renderable_text>([&](auto& countdown, rendering::renderable_text& text)
			{
				countdown.current_value -= m_timer.delta_secs();
				text.string_hash = _strings.hash_and_store(std::to_string((int)countdown.current_value));
				if (countdown.current_value <= 0) {
					countdown.current_value = countdown.count_duration;
					ended = true;
					game_over_event go;
					event_manager.BroadcastEvent(go);
					std::cerr << "Game Over!" << std::endl;
				}
			});
		}
	}

private:
	core::frame_timer& m_timer;
	bool is_first_run = true;
	bool ended = false;
	event::EventManager& event_manager;
	util::string_table& _strings;
	core::startup_config config;
};

#endif