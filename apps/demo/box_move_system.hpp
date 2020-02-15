#ifndef GAME_BOX_MOVE_SYSTEM_HPP
#define GAME_BOX_MOVE_SYSTEM_HPP

#include <transforms/transform.hpp>
#include <rendering/camera.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <collisions/collision_manager.hpp>
#include "components/game_piece.hpp"

class box_move : public ecs::system_base
{
public:
	box_move(core::frame_timer& timer) : m_timer(timer) {}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each< transforms::transform, collisions::AABB_collider, collisions::rigid_body, components::game_piece>([&](transforms::transform& transform, collisions::AABB_collider& sphere_collider, collisions::rigid_body& rigid_body, components::game_piece& game_piece)
			{
				if (transform.position.x <= -520)
				{
					dir = -1.f;
				}

				if (transform.position.x >= -480) {
					dir = 1.f;
				}

				rigid_body.forces.x += 5.f * dir;

				transform.is_matrix_dirty = true;
			});
	}

private:
	float dir = 1.f;
	core::frame_timer& m_timer;
};

#endif