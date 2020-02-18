#ifndef HEALTH_DAMAGE_SYSTEM_HPP
#define HEALTH_DAMAGE_SYSTEM_HPP

#include <transforms/transform.hpp>
#include <rendering/camera.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <collisions/collision_manager.hpp>
#include "components/game_piece.hpp"
#include "components/hit_points.hpp"
#include <glm/glm.hpp>


class health_damage_system : public ecs::system_base
{
public:
	health_damage_system(core::frame_timer& timer) : m_timer(timer) {}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each_id< transforms::transform, components::hit_points>([&](entity_id id1, transforms::transform& transform1, components::hit_points& hp1)
			{
				r_state.each_id< transforms::transform, components::hit_points>([&](entity_id id2, transforms::transform& transform2, components::hit_points& hp2)
					{
						if (id1 != id2) {
							glm::vec3 dist_vec = transform1.position - transform2.position;
							float dist = glm::length(dist_vec);
							if (dist <= hp1.range) {
								hp2.current_hp -= hp1.damage * m_timer.delta_secs();
								if (hp2.current_hp < 0) {
									hp2.current_hp = 0;
								}
							}
						}
					});

				transform1.rotation.z = 3.14f *(1.0f - hp1.current_hp / hp1.max_hp);
				transform1.is_matrix_dirty = true;
			});
		
	}

private:
	core::frame_timer& m_timer;
};

#endif
