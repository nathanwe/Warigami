#ifndef WIZARDPEOPLE_RENDERING_RENDERER_HPP
#define WIZARDPEOPLE_RENDERING_RENDERER_HPP

#include "ecs/state.hpp"
#include "ecs/system_base.hpp"

namespace rendering
{
	class renderer : ecs::system_base
	{
	public:
		virtual void update(ecs::state& state) override;
	};
}

#endif
