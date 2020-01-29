#ifndef WIZARDPEOPLE_RENDERING_CAMERA_UPDATER_HPP
#define WIZARDPEOPLE_RENDERING_CAMERA_UPDATER_HPP

#include "ecs/system_base.hpp"

namespace rendering
{
	class camera_updater : public ecs::system_base
	{
		virtual void update(ecs::state& state) override;
	};
}

#endif
