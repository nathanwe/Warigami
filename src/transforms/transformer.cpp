#include "transforms/transformer.hpp"

#include "glm/gtx/euler_angles.hpp"

namespace transforms
{
	void transformer::update(ecs::state& state)
	{
		state.each<transform>([&](auto id, auto& r_transform) 
		{
			clean_transform_hierarchy(state, id, r_transform);
		});
	}

	glm::mat4 calculate_matrix(transform& r_transform)
	{
		return glm::translate(r_transform.position) *
			glm::eulerAngleZXY(r_transform.rotation.z, r_transform.rotation.x, r_transform.rotation.y) *
			glm::scale(r_transform.scale);
	}

	void transformer::clean_transform_hierarchy(ecs::state& r_state, entity_id id, transform& r_transform)
	{
		if (r_transform.is_matrix_dirty)
		{
			if (r_transform.parent == SOME_NULL_ENTITY_ID)
			{
				r_transform.local_to_world = calculate_matrix(r_transform);
			}
			else
			{
				// recursively clean dependencies
				auto& parent_entity = r_state.get_entity(r_transform.parent);
				auto& parent_transform = parent_entity.get_component<transform>();
				clean_transform_hierarchy(r_state, r_transform.parent, parent_transform);

				// calculate local transformation
				// this has to be done after recursion since _local_to_parent is shared by all iterations
				//	and would otherwise be overwritten during the recursion
				_local_to_parent = calculate_matrix(r_transform);

				// apply inheritance
				r_transform.local_to_world = parent_transform.local_to_world * _local_to_parent;
			}
			r_transform.is_matrix_dirty = false;
		}
	}
}

#endif
