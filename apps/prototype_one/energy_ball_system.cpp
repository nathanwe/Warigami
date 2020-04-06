#include <glm/gtx/norm.hpp>
#include <transforms/transform.hpp>
#include <collisions/rigid_body.hpp>
#include "energy_ball_system.hpp"
#include "components/energy_ball.hpp"
#include "components/player.hpp"

energy_ball_system::energy_ball_system(
	core::game_input_manager& input,
	asset::scene_hydrater& hydrater,	
	core::frame_timer& timer)
	: _input(input)
	, _hydrater(hydrater)
	, _timer(timer)
{
}

void energy_ball_system::initialize(ecs::state& state)
{
	_players[0] = state.first<components::player>([&](components::player& p) {
		return p.team == 1.f;
	});

	_players[1] = state.first<components::player>([&](components::player& p) {
		return p.team == -1.f;
	});

	_selectors[0] = state.first<components::selection_arrow>([&](components::selection_arrow& sa) {
		return sa.team == 1.f;
	});

	_selectors[1] = state.first<components::selection_arrow>([&](components::selection_arrow& sa) {
		return sa.team == -1.f;
	});
}


void energy_ball_system::update(ecs::state& state)
{
	state.each_id<transforms::transform, components::energy_ball, collisions::rigid_body>(
		[&](entity_id ball_id, transforms::transform& transform, components::energy_ball& ball, collisions::rigid_body& rb) {
			auto selector = ball.team == 1.f
				? _selectors[0]
				: _selectors[1];

			auto p = ball.team == 1.f
				? _players[0]
				: _players[1];

			auto& player_c = p->get_component<components::player>();
			auto& sa = selector->get_component<components::selection_arrow>();

			player_specific_data specifis(player_c, _input);

			if (_input.is_input_active(specifis.controls.dice_button2) || player_c.controlled_by_AI)
			{
				auto target_pos = find_target_position(sa);
				glm::vec3 source_pos = transform.local_to_world[3];
				glm::vec3 to_target = target_pos - source_pos;
				glm::vec3 to_target_norm = glm::normalize(to_target);
				auto lengthsq = glm::length2(to_target);
				auto magnitude = ball.gravity_constant / lengthsq;

				auto fwd = glm::vec3(0, 0, 1) * player_c.team;
				auto past_selector = glm::dot(fwd, -to_target) < 0;

				if (lengthsq < 0.5f || past_selector)
				{
					player_c.energy = glm::min(player_c.energy + 2, player_c.max_energy);
					_hydrater.remove_entity(ball_id);
				}
				else
				{
					rb.forces += to_target_norm * magnitude;
				}
			}
			else
			{
				rb.velocity *= (1.f - 2.f * _timer.smoothed_delta_secs());
			}
		});
}

glm::vec3 energy_ball_system::find_target_position(components::selection_arrow& arrow)
{
	auto& player_e = arrow.team == 1
		? _players[0]
		: _players[1];

	auto& player = player_e->get_component<components::player>();

	for (size_t i = 0; i < arrow.energy_orbs.size(); ++i)
	{
		if (i < player.energy && i < arrow.energy_orbs.size()-1) continue;
		
		auto& orb_t = arrow.energy_orbs[i].get_component<transforms::transform>();
		return orb_t.local_to_world[3];
	}	
}
