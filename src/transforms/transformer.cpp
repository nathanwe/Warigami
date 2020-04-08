#include "transforms/transformer.hpp"

#include "ecs/state.hpp"
#include "transforms/transform.hpp"

#include "glm/gtx/euler_angles.hpp"

namespace transforms
{
	transformer::transformer(asset::scene_hydrater& hydrater) : _hydrater(hydrater)
	{
	}


	void transformer::update(ecs::state& state)
	{
		state.each_id<transform>([&](auto id, auto& r_transform)
		{
			r_transform.just_cleaned = false;
		});
		state.each_id<transform>([&](auto id, auto& r_transform)
		{
			clean_transform_hierarchy(state, id, r_transform);
		});
	};

	glm::mat4 calculate_matrix(transform& r_transform)
	{
		glm::mat4 translation = glm::translate(glm::mat4(1), r_transform.position);
		glm::mat4 rotation    = glm::eulerAngleZXY(r_transform.rotation.z, r_transform.rotation.x, r_transform.rotation.y);
		glm::mat4 scale       = glm::scale(glm::mat4(1), r_transform.scale);
		return translation * rotation * scale;
	}

	void transformer::clean_transform_hierarchy(ecs::state& r_state, entity_id id, transform& r_transform)
	{
		if (!r_transform.has_parent)
		{
			if (r_transform.is_matrix_dirty)
			{
				r_transform.local_to_world = calculate_matrix(r_transform);
				r_transform.is_matrix_dirty = false;
				r_transform.just_cleaned = true;
			}
		}
		else
		{
			if (!r_state.has_entity(r_transform.parent))
			{
				r_transform.has_parent = false;
				_hydrater.remove_entity(id);				
				return;
			}

			// recursively clean dependencies
			auto& parent_entity = r_state.find_entity(r_transform.parent);
			auto& parent_transform = parent_entity.get_component<transform>();
			clean_transform_hierarchy(r_state, r_transform.parent, parent_transform);

			// calculate local transformation
			if (r_transform.is_matrix_dirty)
			{
				r_transform.local_to_parent = calculate_matrix(r_transform);
			}

			// apply inheritance
			if (r_transform.is_matrix_dirty || parent_transform.just_cleaned)
			{
				r_transform.local_to_world = parent_transform.local_to_world * r_transform.local_to_parent;
				r_transform.is_matrix_dirty = false;
				r_transform.just_cleaned = true;
			}
		}
	}
}
