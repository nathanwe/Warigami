#ifndef WIZARD_ABC_HPP
#define WIZARD_ABC_HPP

#include "ecs/state.hpp"
#include <engine-ui/view.hpp>

namespace engineui
{

	class fps_display : public view
	{
	public:
		fps_display(core::viewport& viewport, ecs::state& r_ecs_state);

		void draw();

	private:
		core::frame_timer& _frame_timer;
		ecs::state& m_r_ecs_state;
	};

}

#endif