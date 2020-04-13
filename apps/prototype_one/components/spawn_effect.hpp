#ifndef WIZARD_GAME_COMPONENT_SPAWN_EFFECT_HPP
#define WIZARD_GAME_COMPONENT_SPAWN_EFFECT_HPP

#include <ecs/component.hpp>

namespace components
{
	struct spawn_effect : public ecs::component<spawn_effect>
	{
		static constexpr float s_duration = 1.f;
		static constexpr float s_rate = 1.f;
		static constexpr unsigned int s_num_sprites = 3;

		float time_started = 0.f;
	};
}

#endif