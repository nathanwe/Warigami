#ifndef __ANIMATOR_HPP_
#define __ANIMATOR_HPP_

#include <core/frame_timer.hpp>
#include <ecs/system_base.hpp>
#include <rendering/renderable_mesh_rigged.hpp>

namespace rendering
{
	class animator : public ecs::system_base
	{
	public:
		animator(core::frame_timer& timer);
		
		void update(ecs::state& state);

	private:
		core::frame_timer& _timer;
	};
}

#endif