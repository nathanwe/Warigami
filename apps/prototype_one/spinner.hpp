#ifndef __SPINER_HPP_
#define __SPINER_HPP_

#include <core/frame_timer.hpp>
#include <ecs/state.hpp>
#include <ecs/system_base.hpp>


class spinner : public ecs::system_base
{
public:
	spinner(core::frame_timer& timer) :
		_timer(timer)
	{
	}

	virtual void update(ecs::state& r_state) override
	{
		/*r_state.each< transforms::transform, rendering::renderable_mesh_static>([&](auto& transform, auto& renderable)
			{
				transform.rotation.y += m_rotation_speed * _timer.delta_secs();
				transform.is_matrix_dirty = true;
			});*/
	}

private:
	core::frame_timer& _timer;
	float m_rotation_speed = .5f;
};


#endif