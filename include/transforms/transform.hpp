#ifndef WIZARDPEOPLE_TRANSFORMS_TRANSFORM_HPP
#define WIZARDPEOPLE_TRANSFORMS_TRANSFORM_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "transforms/component_bits.hpp"

#include <glm/glm.hpp>

namespace transforms
{
	struct transform : ecs::component<transform>
	{
		glm::vec3 position        = glm::vec3(0);
		glm::vec3 prev_position	  = glm::vec3(0);
		glm::vec3 rotation        = glm::vec3(0);
		glm::vec3 scale           = glm::vec3(1);
		glm::mat4 local_to_world  = glm::mat4(0);
		glm::mat4 local_to_parent = glm::mat4(1);
		bool is_matrix_dirty      = true;
		entity_id parent          = 0;
		bool has_parent           = false;
		bool just_cleaned         = false;
	};
}



//template<> const component_shift ecs::component<transforms::transform>::component_bitshift = (component_shift)transforms::component_bits::transform;

#endif
