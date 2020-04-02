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
	energy_meter_system(
		util::string_table& strings,
		core::glfw_context& config)
		: _strings(strings),
		_glfw_context(config) {}

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
		r_state.each<components::energy_meter, rendering::renderable_text>([&](auto& meter, rendering::renderable_text& text)
		{
			meter.value = 5.f;
			if (meter.team == 1) {
				text.position = glm::ivec2(50, _glfw_context.height() - 150);
			}
			else if (meter.team == -1) {
				text.position = glm::ivec2(_glfw_context.width() - 200, _glfw_context.height() - 150);
			}
			text.scale = 3.0;
			std::ostringstream oss;
			oss << std::setfill('0') << std::setw(2) << meter.value;
			text.string_hash = _strings.hash_and_store(oss.str());
		});
	}

	void not_first_run(ecs::state& r_state)
	{
		r_state.each<components::energy_meter, rendering::renderable_text>([&](auto& meter, rendering::renderable_text& text)
		{
			float clamped = 0.f;
			r_state.each<components::player>([&](auto& p)
			{
				if (p.team == meter.team)
				{
					clamped = std::clamp(p.energy, 0.0f, 10.0f);
				}

			});
			meter.value = clamped;
			std::ostringstream oss;
			oss << std::setfill('0') << std::setw(2) << (int)meter.value;
			text.string_hash = _strings.hash_and_store(oss.str());
		});
	}

private:
	bool is_first_run = true;
	util::string_table& _strings;
	core::glfw_context& _glfw_context;
};

#endif