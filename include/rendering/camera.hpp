#ifndef WIZARDPEOPLE_RENDERING_CAMERA_HPP
#define WIZARDPEOPLE_RENDERING_CAMERA_HPP

#include "ecs/component.hpp"
#include "rendering/cube_map.hpp"

#include "glm/vec3.hpp"

namespace rendering
{
	struct camera : ecs::component<camera>
	{
		enum class clear_mode { none, flat_color, sky_box };

		float field_of_view      = 0;
		float aspect_ratio       = 0;
		float near_clip_distance = 0;
		float far_clip_distance  = 0;

		bool is_view_dirty = true;
		bool is_projection_dirty = true;

		glm::mat4 view                    = glm::mat4(0);
		glm::mat4 projection              = glm::mat4(0);
		glm::mat4 view_projection         = glm::mat4(0);
		glm::mat4 inverse_view            = glm::mat4(0);
		glm::mat4 inverse_projection      = glm::mat4(0);
		glm::mat4 inverse_view_projection = glm::mat4(0);

		glm::vec3 clear_color = glm::vec3(0);
		cube_map* clear_sky_box = nullptr;
	};
}

template<> const component_shift ecs::component<rendering::camera>::component_bitshift = (component_shift)rendering::component_bits::camera;

#endif
