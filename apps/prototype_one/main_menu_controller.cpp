#include <rendering/renderable_text.hpp>

#include "main_menu_controller.hpp"
#include "components/main_menu.hpp"

#include <transforms/transform.hpp>
#include <asset/scene_change_event.hpp>

main_menu_controller::main_menu_controller(
	event::EventManager& events, 
	core::glfw_context& glfw,
	core::game_input_manager& input)
	: _events(events)
	, _glfw(glfw)
	, _input(input)
{
}

void main_menu_controller::initialize(ecs::state& state)
{
}

void main_menu_controller::update(ecs::state& state)
{
	auto half_height = _glfw.height() / 2.f;
	auto half_width = _glfw.width() / 2.f;


	state.each<components::main_menu>([&](components::main_menu& menu) {
		auto& start = state.find_entity(101);
		auto& howto = state.find_entity(102);
		auto& options = state.find_entity(103);
		auto& exit = state.find_entity(108);
		
		auto& start_text = start.get_component<rendering::renderable_text>();
		auto& howto_text = howto.get_component<rendering::renderable_text>();
		auto& options_text = options.get_component<rendering::renderable_text>();
		auto& exit_text = exit.get_component<rendering::renderable_text>();
		
		options_text.position.x = _glfw.width() - 272;
		options_text.position.y = 8;

		howto_text.position.x = 8;
		howto_text.position.y = 8;

		exit_text.position.x = _glfw.width() - 280;
		exit_text.position.y = _glfw.height() - 64;


		if (_input.is_input_ended(core::controls::START))
		{
			asset::scene_change_event game_start("assets/scenes/scene.json");
			_events.BroadcastEvent(game_start);
		}

		if (_input.is_input_ended(core::controls::CARD1_CONTROL))
		{
			menu.how_to_page++;
			if (menu.how_to_page > 3)
			{
				menu.how_to_page = components::main_menu::NoPage;
			}
		}

		if (menu.how_to_page == components::main_menu::NoPage)
		{

			start_text.position.x = half_width - 150;
			start_text.position.y = half_height + 64;

			for (size_t i = 0; i < 4; ++i)
			{
				auto& e = state.find_entity(menu.how_to_play_images[i]);
				auto& t = e.get_component<transforms::transform>();
				t.position = HidePosition;
				t.is_matrix_dirty = true;
			}
		}
		else
		{
			start_text.position.x = HidePosition.x;
			start_text.position.y = HidePosition.y;

			for (size_t i = 0; i < 4; ++i)
			{
				auto& e = state.find_entity(menu.how_to_play_images[i]);
				auto& t = e.get_component<transforms::transform>();
				t.position = i == menu.how_to_page
					? glm::vec3(0.f, 0.f, 0.f)
					: HidePosition;
				t.is_matrix_dirty = true;
			}			
		}

	});
}
