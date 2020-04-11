#ifndef __LOGO_HPP_
#define __LOGO_HPP_

#include <ecs/component.hpp>

namespace components
{
	struct logo : public ecs::component<logo>
	{
		float duration{ 2.f };
		float display_time{ 0.f };
		float fade_time_total { 0.5f };
		float fade_time{ 0.f };
	};

}

#endif