#ifndef WIZARD_GAME_COUNTDOWN_SYSTEM_HPP
#define WIZARD_GAME_COUNTDOWN_SYSTEM_HPP

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/frame_timer.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <util/string_table.hpp>


#include "components/countdown.hpp"
#include "components/pause.hpp"

#include <algorithm>

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
		r_state.each<components::countdown>([&](components::countdown& countdown)
		{
			// Prepare for start countdown
			countdown.current_value = START_COUNT_SECONDS;
		});
		_board = r_state.first<components::board>();
	}

	// Note: Magic numbers in here are what looks good, chosen by looking at results
	void update(ecs::state& r_state) override
	{		
		if (_board == nullptr) return;
		auto& board_component = _board->get_component<components::board>();

		// Count before starting (3, 2, 1, GO!)
		if (board_component.state == components::game_state::countdown) 
		{
			r_state.each<components::countdown, rendering::renderable_mesh_static>(
				[&](auto& countdown, auto& render)
				{
					render.is_enabled = true;

					// {4, 3, 2, 1, 0}
					auto floored_countdown = static_cast<int>(countdown.current_value);

					// {3, 2, 1, 0}
					floored_countdown = std::clamp(floored_countdown, 0, START_COUNT_SECONDS - 1);

					// {1.f, 0.75f, 0.5f, 0.25f}
					auto stepped_t = static_cast<float>(floored_countdown + 1) / START_COUNT_SECONDS;

					// {0.f, 0.25f, 0.5f, 0.75f}
					auto flipped_stepped_t = 1.f - stepped_t;
					render.material.texture_offset.x = flipped_stepped_t;

					if (countdown.current_value < 1.f)
					{
						board_component.state = components::game_state::gameplay;
						r_state.each<components::pause>([&](auto& pause)
							{
								pause.is_game_countdown_over = true;
							});
					}
					countdown.current_value -= m_timer.delta_secs();
				});
		}
		else if (board_component.state == components::game_state::gameplay)
		{
			r_state.each_id<components::countdown>(
				[&](auto id, auto& countdown)
				{
					if (countdown.current_value <= 0)
					{
						r_state.remove_entity(r_state.find_entity(id));
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
};

#endif