#include <glm/gtx/norm.hpp>
#include <transforms/transform.hpp>
#include <collisions/rigid_body.hpp>
#include <platform/rumble.hpp>
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
	cancel_rumble();

	state.each_id<transforms::transform, components::energy_ball, collisions::rigid_body>(
		[&](entity_id ball_id, transforms::transform& transform, components::energy_ball& ball, collisions::rigid_body& rb) {

			auto player_index = ball.team == 1.f ? 0 : 1;
			auto selector = _selectors[player_index];
			auto player_entity = _players[player_index];

			auto& player_c = player_entity->get_component<components::player>();
			auto& sa = selector->get_component<components::selection_arrow>();

			player_specific_data specifis(player_c, _input);

			auto fwd = glm::vec3(0, 0, 1) * player_c.team;
			auto target_pos = find_target_position(sa);
			glm::vec3 source_pos = transform.local_to_world[3];
			glm::vec3 to_target = target_pos - source_pos;
			glm::vec3 to_target_norm = glm::normalize(to_target);
			auto lengthsq = glm::length2(to_target);
			auto magnitude = std::max(200.f, ball.gravity_constant / lengthsq);
			auto past_selector = glm::dot(fwd, -to_target_norm) < 0;
			auto is_same_lane = ball.lane == player_c.selected_row;
			auto pull_pressed = specifis.values.pull > 0.4f || player_c.controlled_by_AI;
			auto can_pull = is_same_lane && pull_pressed;

			if (can_pull)
			{
				rb.forces += to_target_norm * magnitude;
				rb.forces.x = 0;
			}
			else
			{
				rb.velocity *= (1.f - 2.f * _timer.smoothed_delta_secs());
			}

			if (lengthsq < 0.5f || past_selector)
			{
				player_c.energy = glm::min(player_c.energy + 2, player_c.max_energy);
				_hydrater.remove_entity(ball_id);
				os::rumble(player_index, RumbleStrength, RumbleStrength);
				cancel_rumble.reset();
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

	return glm::vec3(9999.f);
}
