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

void logo_system::update(ecs::state& state)
{
	state.each<transforms::transform, components::logo, rendering::renderable_mesh_static>(
		[&](transforms::transform& transform, components::logo& logo, rendering::renderable_mesh_static& mesh) {
			logo.display_time += _timer.smoothed_delta_secs();
			
			if (_input.any_button_pressed())
			{
				asset::scene_change_event menu_scene("assets/scenes/main_menu.json");
				_events.BroadcastEvent(menu_scene);
				return;
			}

			if (logo.display_time > logo.duration)
			{
				logo.fade_time += _timer.smoothed_delta_secs();
				auto fade_t = logo.fade_time / logo.fade_time_total;
				mesh.material.tint_color = glm::mix(glm::vec3(1.f), glm::vec3(0.f), fade_t);

				if (fade_t > 0.95f)
				{
					mesh.material.tint_color = glm::vec3(0.f);
					transform.position = glm::vec3(99999.f);
					transform.is_matrix_dirty = true;
				}

				if (logo.fade_time > logo.fade_time_total)
				{					
					asset::scene_change_event menu_scene("assets/scenes/main_menu.json");
					_events.BroadcastEvent(menu_scene);
				}
			}
		});
}
