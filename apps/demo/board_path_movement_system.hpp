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


class board_path_movement_system : public ecs::system_base
{
public:
	board_path_movement_system(core::frame_timer& timer) : m_timer(timer) {}

	void add_path(ecs::state& r_state, int subject_id, std::list<glm::vec2> locations) {
		glm::vec3 board_position, board_scale = { 0,0,0 };
		int sanity_check = 0;
		r_state.each<transforms::transform, components::board>([&](transforms::transform& transform, components::board& board)
			{
				board_position = transform.position;
				board_scale = transform.scale;
				sanity_check++;
			});
		assert(sanity_check == 1);
		if (r_state.has_entity(subject_id)) {
			ecs::entity& foo = r_state.find_entity(subject_id);
			if (foo.has<components::board_path_movement>()) {
				components::board_path_movement& bp = foo.get_component< components::board_path_movement>();
				assert(bp.path.size() == 0);

				glm::vec3 true_location;
				std::list<glm::vec2>::iterator it;
				for (it = locations.begin(); it != locations.end(); it++) {
					true_location.x = it->x * board_scale.x + board_position.x;
					true_location.z = it->y * board_scale.z + board_position.z;
					true_location.y = 0 * board_scale.y + board_position.y;
					bp.path.push_back(true_location);
				}

			}

		}




	}
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
					float move_distance = glm::length(scaled_move);
					float target_distance = glm::length(movement_direction);
					if (move_distance < target_distance) {
						transform.position += scaled_move;
					}
					else {
						transform.position += movement_direction;
					}
					transform.is_matrix_dirty = true;
				}
					
				
			});
		// move in loops systme
		r_state.each< transforms::transform, components::board_path_movement>([&](transforms::transform& transform, components::board_path_movement& board_path)
			{
				
				
				std::list<glm::vec2> locations = { { 3,1 }, { -2,0 }, { -2, 1 }, { -2, 3}, {-2, 0} };
				if (board_path.path.size() == 0)
				{
					add_path(r_state, 10, locations);
				}

					

			});
	}

private:
	core::frame_timer& m_timer;
};

#endif
