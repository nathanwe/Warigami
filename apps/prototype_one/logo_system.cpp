#include "logo_system.hpp"
#include <rendering/renderable_mesh_static.hpp>
#include <transforms/transform.hpp>
#include "components/logo.hpp"
#include <glm/glm.hpp>

#include <asset/scene_change_event.hpp>

logo_system::logo_system(
	core::frame_timer& timer,
	event::EventManager& events,
	core::game_input_manager& input)
	: _timer(timer)
	, _events(events)
	, _input(input)
{
}

void logo_system::initialize(ecs::state& state)
{
	if (!state.has_entity(200)) return;

	auto& logo = state.find_entity(200);
	logo.get_component<components::logo>().state = components::logo_screen_state::showing;

	auto& controller = state.find_entity(202);
	controller.get_component<components::logo>().does_fade = false;

}

void logo_system::update(ecs::state& state)
{
	bool skipped = false;

	state.each<transforms::transform, components::logo, rendering::renderable_mesh_static>(
		[&](transforms::transform& transform, components::logo& logo, rendering::renderable_mesh_static& mesh) {
			mesh.is_enabled = logo.state == components::logo_screen_state::showing;

			if (logo.state == components::logo_screen_state::showing && !skipped)
				logo.display_time += _timer.smoothed_delta_secs();
			else
				return;
			
			if (_input.any_button_pressed())
			{
				play_next(state, logo, mesh);
				skipped = true;
				return;
			}

			if (logo.display_time > logo.duration && logo.state == components::logo_screen_state::showing)
			{
				logo.fade_time += _timer.smoothed_delta_secs();
				auto fade_t = logo.fade_time / logo.fade_time_total;
				mesh.material.tint_color = logo.does_fade 
					? glm::mix(glm::vec3(1.f), glm::vec3(0.f), fade_t)
					: glm::vec3(1.f);

				if (fade_t > 0.95f)
				{
					mesh.material.tint_color = glm::vec3(0.f);
					mesh.is_enabled = false;
				}

				if (logo.fade_time > logo.fade_time_total)
				{					
					play_next(state, logo, mesh);
				}
			}
		});
}

void logo_system::play_next(ecs::state& state, components::logo& logo, rendering::renderable_mesh_static& mesh)
{
	logo.state = components::logo_screen_state::done;
	mesh.is_enabled = false;

	auto next = state.first<components::logo>([&](components::logo& logo) {
		return logo.state == components::logo_screen_state::waiting;
	});

	if (next)
	{
		auto& next_logo = next->get_component<components::logo>();
		next_logo.state = components::logo_screen_state::showing;
	}
	else
	{
		asset::scene_change_event menu_scene("assets/scenes/main_menu.json");
		_events.BroadcastEvent(menu_scene);
	}
}