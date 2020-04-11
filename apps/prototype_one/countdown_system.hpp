#ifndef WIZARD_GAME_COUNTDOWN_SYSTEM_HPP
#define WIZARD_GAME_COUNTDOWN_SYSTEM_HPP

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/frame_timer.hpp>
#include <util/string_table.hpp>

#include "components/countdown.hpp"
#include "components/pause.hpp"

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
		util::string_table& strings,
        core::glfw_context& config)
		: m_timer(timer),
          event_manager(_event_manager),
          _strings(strings),
          _glfw_context(config) {}


	void initialize(ecs::state& r_state) override
	{
		r_state.each<components::countdown, rendering::renderable_text>([&](components::countdown& countdown, rendering::renderable_text& text)
		{
			// Prepare for start countdown
			countdown.current_value = START_COUNT_SECONDS;
			text.position = glm::ivec2(_glfw_context.width() / 2 - 100, _glfw_context.height() / 2);
			text.scale = 5.0;
			std::ostringstream oss;
			oss << (int)countdown.current_value << "!";
		});

		_board = r_state.first<components::board>();
		_go_string = _strings.hash_and_store("GO!");
	}

	// Note: Magic numbers in here are what looks good, chosen by looking at results
	void update(ecs::state& r_state) override
	{		
		if (_board == nullptr) return;

		auto& board_component = _board->get_component<components::board>();

		// Count before starting (3, 2, 1, GO!)
		if (board_component.state == components::game_state::countdown) {
			r_state.each_id<components::countdown, rendering::renderable_text>([&](auto id, auto& countdown, rendering::renderable_text& text)
			{
				if (countdown.current_value <= 0) {
					// Prepare for the real countdown
					countdown.current_value = countdown.count_duration;
					text.position = glm::ivec2(_glfw_context.width() / 2 - 60, _glfw_context.height() - 60);
					text.scale = 1.5;
					is_start_count = false;
					game_start_event gs;
					event_manager.BroadcastEvent(gs);
					std::cerr << "Game Start!" << std::endl;
				}
				else if (countdown.current_value < 1) {
					// Show "GO!"
					text.position = glm::ivec2(_glfw_context.width() / 2 - 30*text.scale, _glfw_context.height() / 2);
					text.scale += 0.02;
					text.string_hash = _go_string;
					board_component.state = components::game_state::gameplay;
					r_state.each<components::pause>([&](auto& pause)
						{
							pause.is_game_countdown_over = true;
						});
					r_state.remove_entity(r_state.find_entity(id));
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
	}

private:
	core::frame_timer& m_timer;	
	bool is_start_count = true;
	bool ended = false;
	event::EventManager& event_manager;
	util::string_table& _strings;
	core::glfw_context& _glfw_context;

	ecs::entity* _board {nullptr};
	size_t _go_string;
};

#endif