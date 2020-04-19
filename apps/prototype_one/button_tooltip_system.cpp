#include <rendering/renderable_mesh_static.hpp>

#include "button_tooltip_system.hpp"
#include "components/button_tooltip.hpp"

void button_tooltip_system::initialize(ecs::state& state)
{
	_players[0] = state.first<components::player>([&](components::player& p) {
		return p.team == 1.f;
	});

	_players[1] = state.first<components::player>([&](components::player& p) {
		return p.team == -1.f;
	});
}

void button_tooltip_system::update(ecs::state& state)
{
	if (_players[0] == nullptr) return;

	auto& p1 = _players[0]->get_component<components::player>();
	auto& p2 = _players[1]->get_component<components::player>();

	state.each<components::button_tooltip, rendering::renderable_mesh_static>(
		[&](components::button_tooltip& bt, rendering::renderable_mesh_static& rms) {
			auto& p = bt.team == 1.f ? p1 : p2;
			auto card = p.hand[bt.hand_index];
			auto cost = components::card_costanamos[(int)card];

			if (cost > p.energy)
			{
				rms.material.tint_color = glm::vec3(0.5f);
				rms.material.tint_alpha = 0.5f;
				//rms.material.grayscale = true;
			}
			else
			{
				rms.material.tint_color = glm::vec3(1.f);
				rms.material.tint_alpha = 1.f;
				//rms.material.grayscale = false;
			}
	});
}
