#ifndef __DECK_CURSOR_HPP_
#define __DECK_CURSOR_HPP_

#include <ecs/component.hpp>

namespace components
{
	struct deck_cursor : public ecs::component<deck_cursor>
	{
		float team{ 1.f };
	};
}

#endif