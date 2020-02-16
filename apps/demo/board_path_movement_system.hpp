#ifndef BOARD_PATH_MOVEMENT_SYSTEM_HPP
#define BOARD_PATH_MOVEMENT_SYSTEM_HPP

#include <transforms/transform.hpp>
#include <rendering/camera.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <collisions/collision_manager.hpp>
#include "components/game_piece.hpp"
#include "components/board_path_movement.hpp"
#include <glm/glm.hpp>

#define EPSILON 0.000001f

float sq_dist(glm::vec3 in) {
	return in.x * in.x + in.y * in.y + in.z * in.z;
}

class board_path_movement_system : public ecs::system_base
{
public:
	board_path_movement_system(core::frame_timer& timer) : m_timer(timer) {}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each< transforms::transform, components::board_path_movement>([&](transforms::transform& transform, components::board_path_movement& board_path)
			{
				while (board_path.path.size() != 0 &&
					transform.position.x + EPSILON >= board_path.path.begin()->x && transform.position.x - EPSILON <= board_path.path.begin()->x &&
					transform.position.y + EPSILON >= board_path.path.begin()->y && transform.position.y - EPSILON <= board_path.path.begin()->y&&
					transform.position.z + EPSILON >= board_path.path.begin()->z && transform.position.z - EPSILON <= board_path.path.begin()->z) {

					board_path.path.pop_front();
				}
				if (board_path.path.size() != 0) {
					glm::vec3 movement_direction =  *(board_path.path.begin()) - transform.position;
					glm::vec3 normal_move = glm::normalize(movement_direction);
					glm::vec3 scaled_move = normal_move * board_path.speed * m_timer.delta_secs();
					float move_distance = sq_dist(scaled_move);
					float target_distance = sq_dist(movement_direction);
					if (move_distance < target_distance) {
						transform.position += scaled_move;
					}
					else {
						transform.position += movement_direction;
					}
					transform.is_matrix_dirty = true;
				}
					
				
			});
	}

private:
	float dir = 1.f;
	core::frame_timer& m_timer;
};

#endif