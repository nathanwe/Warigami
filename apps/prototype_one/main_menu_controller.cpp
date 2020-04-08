#include <rendering/renderable_text.hpp>

#include "main_menu_controller.hpp"
#include "components/main_menu.hpp"

#include <transforms/transform.hpp>
#include <asset/scene_change_event.hpp>

main_menu_controller::main_menu_controller(
	event::EventManager& events, 
	core::glfw_context& glfw,
	core::game_input_manager& input,
	core::frame_timer& timer)
	: _events(events)
	, _glfw(glfw)
	, _input(input)
	, _timer(timer)
{
}

void main_menu_controller::initialize(ecs::state& state)
{
}

void main_menu_controller::update(ecs::state& state)
{
	state.each<components::main_menu>([&](components::main_menu& menu) {

		if (menu.how_to_page == components::main_menu::NoPage)
			handle_main_menu_case(state, menu);
		else
			handle_howto_case(state, menu);

		auto& board = state.find_entity(69);
		auto& transform = board.get_component<transforms::transform>();
		transform.rotation.y += 0.4f * _timer.delta_secs();
		transform.is_matrix_dirty = true;
	});
}

void main_menu_controller::handle_howto_case(ecs::state& state, components::main_menu& menu)
{
	texts menu_items(state);

	menu_items.start_text.position.x = HidePosition.x;
	menu_items.start_text.position.y = HidePosition.y;
	menu_items.exit_text.position.x = HidePosition.x;
	menu_items.exit_text.position.y = HidePosition.y;
	menu_items.options_text.position.x = HidePosition.x;
	menu_items.options_text.position.y = HidePosition.y;
	menu_items.howto_text.position.x = HidePosition.x;
	menu_items.howto_text.position.y = HidePosition.y;

	for (size_t i = 0; i < 4; ++i)
	{
		auto& e = state.find_entity(menu.how_to_play_images[i]);
		auto& t = e.get_component<transforms::transform>();
		t.position = i == menu.how_to_page
			? glm::vec3(0.f, 0.f, 0.f)
			: HidePosition;
		t.is_matrix_dirty = true;
	}

	if (_input.any_button_pressed())
	{
		menu.how_to_page++;
		if (menu.how_to_page > 3)
			menu.how_to_page = components::main_menu::NoPage;
	}
}

void main_menu_controller::handle_main_menu_case(ecs::state& state, components::main_menu& menu)
{
	texts menu_items(state);
	auto half_height = _glfw.height() / 2.f;
	auto half_width = _glfw.width() / 2.f;

	if (_input.is_input_ended(core::controls::START))
	{
		asset::scene_change_event game_start("assets/scenes/scene.json");
		_events.BroadcastEvent(game_start);
	}

	if (_input.is_input_started(core::controls::CARD1_CONTROL))
	{
		menu.how_to_page++;	
	}

	menu_items.start_text.position.x = half_width - 150;
	menu_items.start_text.position.y = half_height + 64;
	menu_items.exit_text.position.x = 8;
	menu_items.exit_text.position.y = 8;
	menu_items.options_text.position.x = 8;
	menu_items.options_text.position.y = 64;
	menu_items.howto_text.position.x = 8;
	menu_items.howto_text.position.y = 128;

	for (size_t i = 0; i < 4; ++i)
	{
		auto& e = state.find_entity(menu.how_to_play_images[i]);
		auto& t = e.get_component<transforms::transform>();
		t.position = HidePosition;
		t.is_matrix_dirty = true;
	}
}