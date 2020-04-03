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

	bool transformer::clean_transform_hierarchy(ecs::state& r_state, entity_id id, transform& r_transform)
	{
		bool did_change = false;
		if (!r_state.has_entity(r_transform.parent))
		{
			if (r_transform.is_matrix_dirty)
			{
				r_transform.local_to_world = calculate_matrix(r_transform);
				r_transform.is_matrix_dirty = false;
				did_change = true;
			}			
		}
		else
		{
			// recursively clean dependencies
			auto& parent_entity = r_state.find_entity(r_transform.parent);
			auto& parent_transform = parent_entity.get_component<transform>();
			auto did_ancestor_change = clean_transform_hierarchy(r_state, r_transform.parent, parent_transform);

			// calculate local transformation
			if (r_transform.is_matrix_dirty)
			{
				r_transform.local_to_parent = calculate_matrix(r_transform);
			}

			// apply inheritance
			if (r_transform.is_matrix_dirty || did_ancestor_change)
			{
				r_transform.local_to_world = parent_transform.local_to_world * r_transform.local_to_parent;
				r_transform.is_matrix_dirty = false;
				did_change = true;
			}
		}
		return did_change;
	}
}

void transforms::transformer::remove_if_orphaned(entity_id id, transforms::transform& r_transform)
{
	if (r_transform.parent != ecs::default_entity_id)
	{
		_hydrater.remove_entity(id);
		return;
	}
}