#ifndef __ENERGY_BALL_HPP_
#define __ENERGY_BALL_HPP_

#include <ecs/component.hpp>

namespace components
{
	struct energy_ball : public ecs::component<energy_ball>
	{
		float gravity_constant{ 600.f };
		float team{ 1.f };
	};
}

#endif