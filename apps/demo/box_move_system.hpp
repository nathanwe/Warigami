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
	box_move(core::frame_timer& timer, core::game_input_manager& input) : m_timer(timer), m_input(input) {}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each< transforms::transform, collisions::AABB_collider, collisions::rigid_body, components::card, audio::audio_emitter>
		([&](
			transforms::transform& transform, 
			collisions::AABB_collider& aabb_collider, 
			collisions::rigid_body& rigid_body, 
			components::card& card, 
			audio::audio_emitter& emitter)
			{
				if (card.timer <= 0.f)
				{
					rigid_body.forces.y += 20000.f;
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
					card.timer = 5.f;
				}
				else
				{
					card.timer -= m_timer.smoothed_delta_secs();
				}
				
				transform.is_matrix_dirty = true;
			});
	}

private:
	glm::vec4 dir;
	int lap = 0;
	core::frame_timer& m_timer;
	core::game_input_manager& m_input;
};

#endif

/*
if (m_input.is_input_active(core::controls::ACTION2_CONTROL))
				{
					if (lap == 0 && transform.rotation.y >= glm::radians(360.f))
					{
						lap = 1;
					}

					if (lap == 1 && transform.rotation.y <= glm::radians(0.f))
					{
						lap = 0;
					}


					if (lap == 0)
					{
						transform.rotation.y += glm::radians(90.f) * m_timer.smoothed_delta_secs();
					}
					if (lap == 1)
					{
						transform.rotation.y -= glm::radians(90.f) * m_timer.smoothed_delta_secs();
					}

					std::cerr << glm::degrees(transform.rotation.y) << std::endl;

					dir = glm::vec4(0, 0, 1, 1) * transform.local_to_world;

					rigid_body.forces += 20.f * glm::vec3(dir);
				}
*/