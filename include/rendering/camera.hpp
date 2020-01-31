#ifndef WIZARDPEOPLE_RENDERING_CAMERA_HPP
#define WIZARDPEOPLE_RENDERING_CAMERA_HPP

#include "ecs/component.hpp"
#include "rendering/component_bits.hpp"
#include "rendering/cube_map.hpp"

#include "glm/glm.hpp"

namespace rendering
{
	struct camera : ecs::component<camera>
	{
		enum class clear_mode { none, flat_color, sky_box };

		float field_of_view      = 1.5708f; // 90 degrees in radians
		float aspect_ratio       = 16.f / 9.f;
		float near_clip_distance = 0.1f;
		float far_clip_distance  = 100.f;

		bool is_view_dirty = true;
		bool is_projection_dirty = true;

		glm::mat4 view                    = glm::mat4(1);
		glm::mat4 projection              = glm::mat4(1);
		glm::mat4 view_projection         = glm::mat4(1);
		glm::mat4 inverse_view            = glm::mat4(1);
		glm::mat4 inverse_projection      = glm::mat4(1);
		glm::mat4 inverse_view_projection = glm::mat4(1);

		clear_mode clear_setting = clear_mode::flat_color;
		glm::vec3 clear_color = glm::vec3(0);
		cube_map clear_sky_box;
	};
}

#endif
