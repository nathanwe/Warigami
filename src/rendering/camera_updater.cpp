#include "rendering/camera_updater.hpp"

#include "rendering/camera.hpp"
#include "transforms/transform.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace rendering
{
	void camera_updater::update(ecs::state& state)
	{
		state.each<transforms::transform, camera>([&](auto& r_transform, auto& r_camera) 
		{
			if (r_camera.is_view_dirty)
			{
				// doesn't handle inheritance
				// maybe cache these steps in transform component
				glm::mat4 translation = glm::translate(glm::mat4(1), r_transform.position);
				glm::mat4 rotation = glm::eulerAngleZYX(r_transform.rotation.z, r_transform.rotation.y, r_transform.rotation.x);
// 				r_camera.inverse_view = translation * rotation;
// 				r_camera.view = glm::inverse(r_camera.inverse_view);
				r_camera.view = translation * rotation;
				r_camera.inverse_view = glm::inverse(r_camera.inverse_view);
			}
			if (r_camera.is_projection_dirty)
			{
				// maybe need an option or separate component for orthographic
				r_camera.projection = glm::perspective(r_camera.field_of_view, r_camera.aspect_ratio, r_camera.near_clip_distance, r_camera.far_clip_distance);
				r_camera.inverse_projection = glm::inverse(r_camera.projection);
			}
			if (r_camera.is_view_dirty || r_camera.is_projection_dirty)
			{
				r_camera.view_projection = r_camera.projection * r_camera.view;
				r_camera.inverse_view_projection = glm::inverse(r_camera.view_projection);

				r_camera.is_view_dirty = false;
				r_camera.is_projection_dirty = false;
			}
		});
	}
}