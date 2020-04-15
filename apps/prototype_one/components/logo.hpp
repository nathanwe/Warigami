#ifndef __LOGO_HPP_
#define __LOGO_HPP_

#include <ecs/component.hpp>

namespace components
{
	enum class logo_screen_state
	{
		waiting, 
		showing,
		done
	};

	struct logo : public ecs::component<logo>
	{
		float duration{ 4.f };
		float display_time{ 0.f };
		float fade_time_total { 0.5f };
		float fade_time{ 0.f };

		bool does_fade{ true };

		logo_screen_state state{ logo_screen_state::waiting };
	};

}

#endif