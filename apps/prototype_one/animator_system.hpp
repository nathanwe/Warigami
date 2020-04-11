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
			render.material.texture_scale = get_sprite_scale(piece);
			if (try_change_sprite(piece))
			{
				render.material.texture_offset = get_sprite_offset(piece);
				piece.last_rendered_state = piece.state;
			}
		});
	}

private:
	bool try_change_sprite(components::game_piece& piece)
	{
		bool changedSprite = false;
		bool changedState = piece.state != piece.last_rendered_state;
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
		return changedSprite;
	}

	void loop_sprite(components::game_piece& piece, int steps)
	{
		piece.last_sprite += steps;
		switch (piece.piece_type)
		{
		case components::card_enum::SCISSORLING_EGG:
			loop_spider_egg(piece);
			break;
		default:
			loop_spider_walker(piece);
			break;
		}
	}

	void loop_spider_walker(components::game_piece& piece)
	{
		switch (piece.state)
		{
		case components::UNIT_STATE::ATTACK:
			piece.last_sprite %= m_spider_attack_offsets.size();
			break;
		case components::UNIT_STATE::DEAD:
			piece.last_sprite %= m_spider_dead_offsets.size();
			break;
		case components::UNIT_STATE::DYING:
			piece.last_sprite %= m_spider_dying_offsets.size();
			break;
		case components::UNIT_STATE::MOVE:
			piece.last_sprite %= m_spider_move_offsets.size();
			break;
		}
	}

	void loop_spider_egg(components::game_piece& piece)
	{
		piece.last_sprite %= m_egg_offsets.size();
	}

	glm::vec2 get_sprite_offset(const components::game_piece& piece)
	{
		switch (piece.piece_type)
		{
		case components::card_enum::SCISSORLING_EGG:
			return get_egg_sprite(piece);
		default:
			return get_walker_sprite(piece);
		}
	}

	glm::vec2 get_walker_sprite(const components::game_piece& piece)
	{
		glm::vec2 offset(0.f);
		switch (piece.state)
		{
		case components::UNIT_STATE::ATTACK:
			offset = m_spider_attack_offsets[piece.last_sprite];
			break;
		case components::UNIT_STATE::DEAD:
			offset = m_spider_dead_offsets[piece.last_sprite];
			break;
		case components::UNIT_STATE::DYING:
			offset = m_spider_dying_offsets[piece.last_sprite];
			break;
		case components::UNIT_STATE::MOVE:
			offset = m_spider_move_offsets[piece.last_sprite];
			break;
		}
		if (piece.team > 0.f)
		{
			offset.x += 0.3333f;
		}
		return offset;
	}

	glm::vec2 get_egg_sprite(const components::game_piece& piece)
	{
		return m_egg_offsets[piece.last_sprite];
	}

	glm::vec2 get_sprite_scale(const components::game_piece& piece)
	{
		glm::vec2 tex_scale(0.f);
		if (piece.piece_type == components::card_enum::SCISSORLING_EGG)
		{
			tex_scale = glm::vec2(0.3333f, 1.0f);
		}
		else
		{
			tex_scale = glm::vec2(0.3333f, 0.5f);
		}
		tex_scale.x *= -piece.team;
		return tex_scale;
	}

private:
	core::frame_timer& m_timer;

	static constexpr float m_time_between_sprites = 0.25f;

	static constexpr std::array<glm::vec2, 2> m_spider_attack_offsets { glm::vec2(0.f, 0.f), glm::vec2(0.3333f, 0.f) };
	static constexpr std::array<glm::vec2, 1> m_spider_dead_offsets   { glm::vec2(0.f, 0.5f) };
	static constexpr std::array<glm::vec2, 1> m_spider_dying_offsets  { glm::vec2(0.f, 0.5f) };
	static constexpr std::array<glm::vec2, 3> m_spider_move_offsets   { glm::vec2(0.f, 0.5f), glm::vec2(0.3333f, 0.5f), glm::vec2(0.6666f, 0.5f) };

	static constexpr std::array<glm::vec2, 3> m_egg_offsets { glm::vec2(0.f, 0.f), glm::vec2(0.3333f, 0.f), glm::vec2(0.6666f, 0.f) };
};


#endif
