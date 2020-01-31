#include <transforms/transform.hpp>
#include <rendering/camera.hpp>
#include <core/input_manager.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <core\frame_timer.hpp>


class fly_cam : public ecs::system_base
{
public:
	fly_cam(core::input_manager& input, core::frame_timer& timer) : m_input(input), m_timer(timer) {}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each< transforms::transform, rendering::camera>([&](auto& transform, auto& camera)
		{
			if (m_input.is_key_down(GLFW_KEY_W))
				transform.position.z -= m_timer.smoothed_delta_secs() * 10.f;
			if (m_input.is_key_down(GLFW_KEY_A)) 
				transform.position.x -= m_timer.smoothed_delta_secs() * 10.f;
			if (m_input.is_key_down(GLFW_KEY_S))
				transform.position.z += m_timer.smoothed_delta_secs() * 10.f;
			if (m_input.is_key_down(GLFW_KEY_D))
				transform.position.x += m_timer.smoothed_delta_secs() * 10.f;
			if (m_input.is_key_down(GLFW_KEY_SPACE))
				transform.position.y += m_timer.smoothed_delta_secs() * 10.f;
			if (m_input.is_key_down(GLFW_KEY_LEFT_CONTROL))
				transform.position.y -= m_timer.smoothed_delta_secs() * 10.f;


			transform.is_matrix_dirty = true;
			camera.is_view_dirty = true;
		});
	}

private:
	float m_rotation_speed = .001f;
	core::input_manager& m_input;
	core::frame_timer& m_timer;
};