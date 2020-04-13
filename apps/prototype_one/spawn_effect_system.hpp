#ifndef WIZARD_GAME_spawn_effect_system_HPP
#define WIZARD_GAME_spawn_effect_system_HPP

#include <asset/scene_hydrater.hpp>
#include <audio/audio_system.hpp>
#include <core/frame_timer.hpp>
#include <core/game_input_manager.hpp>
#include <core/glfw_context.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <transforms/transform.hpp>
#include <asset/scene_change_event.hpp>
#include <event/event_manager.hpp>

#include "components/pause.hpp"
#include "components/spawn_effect.hpp"

class spawn_effect_system : public ecs::system_base
{
public:
	spawn_effect_system(core::frame_timer& r_timer) : m_r_timer(r_timer)
	{

	}

	void update(ecs::state& r_state) override
	{
		if (should_skip_update(r_state))
		{
			return;
		}

		r_state.each_id<components::spawn_effect, rendering::renderable_mesh_static>(
			[&](auto id, auto& spawn_effect, auto& render)
			{
				if (!kill_spawn_effect(r_state, id, spawn_effect))
				{
					render.material.texture_offset.x = get_sprite_offset(spawn_effect);
				}
			});
	}

private:
	core::frame_timer& m_r_timer;

	bool should_skip_update(ecs::state& r_state)
	{
		auto p_pause_entity = r_state.first<components::pause>();
		if (p_pause_entity == nullptr)
		{
			return false;
		}
		const auto& pause = p_pause_entity->get_component<components::pause>();
		return !pause.is_game_started || !pause.is_game_countdown_over || pause.is_game_over;
	}

	bool kill_spawn_effect(ecs::state& r_state, entity_id id, const components::spawn_effect& spawn_effect)
	{
		bool should_kill = m_r_timer.total_s() - spawn_effect.time_started > components::spawn_effect::s_duration;
		if (should_kill)
		{
			r_state.remove_entity(r_state.find_entity(id));
		}
		return should_kill;
	}

	float get_sprite_offset(components::spawn_effect& spawn_effect)
	{
		auto t = m_r_timer.total_s() - spawn_effect.time_started;
		auto interpolated = t / components::spawn_effect::s_duration;
		auto sprite_idx = static_cast<unsigned int>(interpolated * components::spawn_effect::s_num_sprites);
		sprite_idx = std::clamp(sprite_idx, 0u, components::spawn_effect::s_num_sprites - 1);

		return static_cast<float>(sprite_idx) / components::spawn_effect::s_num_sprites;
	}
};

#endif
