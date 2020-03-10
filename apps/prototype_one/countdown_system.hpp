#ifndef WIZARD_GAME_COUNTDOWN_SYSTEM_HPP
#define WIZARD_GAME_COUNTDOWN_SYSTEM_HPP

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/frame_timer.hpp>
#include <util/string_table.hpp>

#include "components/countdown.hpp"

#define START_COUNT_SECONDS 4 // Including a "GO!" second

class game_over_event : public event::Event
{
public:
	explicit game_over_event() : event::Event(event::EVENT_TYPE::GAME_OVER)
	{}
};

class game_start_event : public event::Event
{
public:
	explicit game_start_event() : event::Event(event::EVENT_TYPE::GAME_START)
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

	// Note: Magic numbers in here are what looks good, chosen by looking at results
	void update(ecs::state& r_state) override
	{
		if (is_first_run)
		{
			r_state.each<components::countdown, rendering::renderable_text>([&](auto& countdown, rendering::renderable_text& text)
			{
				// Prepare for start countdown
				countdown.current_value = START_COUNT_SECONDS;
				text.position = glm::ivec2(config.width()/2 - 100, config.height()/2);
				text.scale = 5.0;
				std::ostringstream oss;
				oss << (int)countdown.current_value << "!";
			});
			is_first_run = false;
		}
		// Count before starting (3, 2, 1, GO!)
		else if (is_start_count) {
			r_state.each<components::countdown, rendering::renderable_text>([&](auto& countdown, rendering::renderable_text& text)
			{
				if (countdown.current_value <= 0) {
					// Prepare for the real countdown
					countdown.current_value = countdown.count_duration;
					text.position = glm::ivec2(config.width() / 2 - 60, config.height() - 60);
					text.scale = 1.5;
					is_start_count = false;
					game_start_event gs;
					event_manager.BroadcastEvent(gs);
					std::cerr << "Game Start!" << std::endl;
				}
				else if (countdown.current_value < 1) {
					// Show "GO!"
					text.position = glm::ivec2(config.width() / 2 - 30*text.scale, config.height() / 2);
					text.scale += 0.02;
					text.string_hash = _strings.hash_and_store("GO!");
				}
				else {
					text.scale += 0.01;
					std::ostringstream oss;
					oss << (int)countdown.current_value << "!";
					text.string_hash = _strings.hash_and_store(oss.str());
				}
				countdown.current_value -= m_timer.delta_secs();
			});
		}
		else if (!ended)
		{
			r_state.each<components::countdown, rendering::renderable_text>([&](auto& countdown, rendering::renderable_text& text)
			{
				countdown.current_value -= m_timer.delta_secs();
				int minutes = countdown.current_value / 60;
				int seconds = (int)countdown.current_value % 60;
				std::ostringstream oss;
				oss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
				text.string_hash = _strings.hash_and_store(oss.str());
				if (minutes == 0) {
					text.scale = 2.0;
					text.color = glm::vec3(1, 0, 0);
					text.position = glm::ivec2(config.width() / 2 - 100, config.height() - 80);
				}
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
	bool is_start_count = true;
	bool ended = false;
	event::EventManager& event_manager;
	util::string_table& _strings;
	core::startup_config config;
};

#endif