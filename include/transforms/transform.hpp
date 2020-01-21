#ifndef WIZARDPEOPLE_TRANSFORMS_TRANSFORM_HPP
#define WIZARDPEOPLE_TRANSFORMS_TRANSFORM_HPP

#include "ecs/components.hpp"
#include "ecs/ecs_types.hpp"

#include <glm/vec3.hpp>

namespace transforms
{
	struct transform : component<transform>
	{
		ecs::entity_id parent;
		glm::vec3 position       = glm::vec3(0);
		glm::vec3 rotation       = glm::vec3(0);
		glm::vec3 scale          = glm::vec3(0);
		glm::mat4 local_to_world = glm::mat4(0);
		bool is_matrix_dirty     = true;
	};
}

template<> const component_shift ecs::component<transforms::transform>::component_bitshift = (component_shift)transforms::component_bits::transform;

#endif
