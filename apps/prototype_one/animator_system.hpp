#ifndef WIZARD_GAME_ANIMATOR_SYSTEM_HPP
#define WIZARD_GAME_ANIMATOR_SYSTEM_HPP

#include <core/frame_timer.hpp>
#include <ecs/state.hpp>
#include <ecs/system_base.hpp>
#include <rendering/renderable_mesh_static.hpp>

#include "components/game_piece.hpp"

#include <glm/glm.hpp>

#include <array>

class animator_system : public ecs::system_base
{
public:
	animator_system(core::frame_timer& timer) : m_timer(timer)
	{
	}

public:
	void update(ecs::state& state) override
	{
		state.each<components::game_piece, rendering::renderable_mesh_static>([&](auto& piece, auto& render)
		{
			render.material.texture_scale = glm::vec2(0.5f, 0.3333f); // move this to json
			render.material.texture_scale.x *= -piece.team;

			bool changedState = piece.state != piece.last_rendered_state;
			bool changedSprite = false;

			if (changedState)
			{
				piece.time_last_sprite = m_timer.total_s();
				piece.last_sprite = 0;
				changedSprite = true;
			}
			else
			{
				float curr = m_timer.total_s();
				float time_diff = curr - piece.time_last_sprite;
				float steps = time_diff / m_time_between_sprites;
				int floored_steps = static_cast<int>(steps);
				if (floored_steps > 0)
				{
					loop_sprite(piece, floored_steps);
					changedSprite = true;
					piece.time_last_sprite = curr;
				}
			}

			if (changedSprite)
			{
				render.material.texture_offset = get_sprite(piece);
				if (piece.team > 0.f)
				{
					render.material.texture_offset.x = render.material.texture_offset.x == 0.f ? 0.5f : 0.f;
				}
				piece.last_rendered_state = piece.state;
			}
		});
	}
private:
	void loop_sprite(components::game_piece& piece, int steps)
	{
		piece.last_sprite += steps;
		switch (piece.state)
		{
		case components::UNIT_STATE::ATTACK:
			piece.last_sprite %= m_attack_offsets.size();
			break;
		case components::UNIT_STATE::DEAD:
			piece.last_sprite %= m_dead_offsets.size();
			break;
		case components::UNIT_STATE::DYING:
			piece.last_sprite %= m_dying_offsets.size();
			break;
		case components::UNIT_STATE::MOVE:
			piece.last_sprite %= m_move_offsets.size();
			break;
		}
	}

	glm::vec2 get_sprite(components::game_piece& piece)
	{
		switch (piece.state)
		{
		case components::UNIT_STATE::ATTACK:
			return m_attack_offsets[piece.last_sprite];
		case components::UNIT_STATE::DEAD:
			return m_dead_offsets[piece.last_sprite];
		case components::UNIT_STATE::DYING:
			return m_dying_offsets[piece.last_sprite];
		case components::UNIT_STATE::MOVE:
			return m_move_offsets[piece.last_sprite];
		default:
			return { 0.f, 0.f };
		}
	}


private:
	core::frame_timer& m_timer;

	const float m_time_between_sprites = 0.25f;
	const std::array<glm::vec2, 2> m_attack_offsets { glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.f) };
	const std::array<glm::vec2, 1> m_dead_offsets   { glm::vec2(0.f, 0.3333f) };
	const std::array<glm::vec2, 1> m_dying_offsets  { glm::vec2(0.f, 0.3333f) };
	const std::array<glm::vec2, 3> m_move_offsets   { glm::vec2(0.f, 0.3333f), glm::vec2(0.5f, 0.3333f), glm::vec2(0.f, 0.6666f) };
};


#endif
