#ifndef GAME_FLY_CAM_SYSTEM_HPP
#define GAME_FLY_CAM_SYSTEM_HPP

#include <transforms/transform.hpp>
#include <rendering/camera.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>


class fly_cam : public ecs::system_base
{
public:
	fly_cam(core::game_input_manager& input, core::frame_timer& timer) : m_input(input), m_timer(timer) {}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each< transforms::transform, rendering::camera, collisions::rigid_body>([&](transforms::transform& transform, rendering::camera& camera, collisions::rigid_body& rigid_body)
			{
				auto camera_world = camera.inverse_view;
				auto fwd = glm::vec3(camera_world[2]);
				auto right = glm::vec3(camera_world[0]);
				//auto up = glm::vec3(camera_world[1]);

				// Non physics movement for the plebians
				//transform.position += m_input.forward() * fwd * m_timer.smoothed_delta_secs() * 10.f; 
				//transform.position += m_input.strafe() * right * m_timer.smoothed_delta_secs() * 10.f;
				
				// Chad physics flying
				//rigid_body.forces += -m_input.forward() * fwd * 10.f;
				//rigid_body.forces += -m_input.strafe() * right * 10.f;

				// Semi-Chad physics walking
				fwd.y = 0;
				right.y = 0;
				rigid_body.forces += -m_input.forward() * fwd * m_timer.smoothed_delta_secs() * 100.f;
				rigid_body.forces += -m_input.strafe() * right * m_timer.smoothed_delta_secs() * 100.f;

				transform.rotation.y -= m_input.yaw() *  m_timer.smoothed_delta_secs() * .5f;
				transform.rotation.x -= m_input.pitch() * m_timer.smoothed_delta_secs() * .5f;

				if (m_input.is_input_active(core::controls::ACTION1_CONTROL) && JUMP_POWA <= 2000.f) {
					JUMP_POWA += 50.f;
				}

				if (m_input.is_input_ended(core::controls::ACTION1_CONTROL)) {
					rigid_body.forces.y -= JUMP_POWA * 0.5f;
					JUMP_POWA = 0.f;
				}

			
			
			transform.is_matrix_dirty = true;
			camera.is_view_dirty = true;
		});
	}

private:
	float m_rotation_speed = .001f;
	float JUMP_POWA = 0.0f;
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
};

#endif