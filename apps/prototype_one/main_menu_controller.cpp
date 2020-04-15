#include <rendering/renderable_text.hpp>

#include "main_menu_controller.hpp"
#include "components/main_menu.hpp"
#include "components/pause_arrow.hpp"

#include <rendering/renderable_mesh_static.hpp>
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
	_selection = 0;
	_option_selection = 0;
	_warning_selection = 0;
	_seeing_new_menu = false;
}

void main_menu_controller::update(ecs::state& state)
{
	state.each<components::main_menu>([&](components::main_menu& menu) {
		auto arrow = state.first<components::pause_arrow>();
		if (arrow == nullptr)
		{
			return;
		}
		auto& arrow_transform = arrow->get_component<transforms::transform>();

		if (!_seeing_new_menu && menu.how_to_page == components::main_menu::NoPage)
			handle_main_menu_case(state, menu, arrow_transform);
		else if (_seeing_new_menu) {
			if (_selection == OPTIONS) {
				handle_options_case(state, menu, arrow_transform);
			}
			else if (_selection == QUIT) {
				handle_quit_case(state, menu, arrow_transform);
			}
		}
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

	menu_items.start_text.position = HidePosition;	
	menu_items.exit_text.position = HidePosition;	
	menu_items.options_text.position = HidePosition;
	menu_items.howto_text.position = HidePosition;	

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

void main_menu_controller::handle_options_case(ecs::state& state, components::main_menu& menu, transforms::transform& arrow_transform) {
	//
}

void main_menu_controller::handle_quit_case(ecs::state& state, components::main_menu& menu, transforms::transform& arrow_transform) {
	auto& e = state.find_entity(111); // Warning image
	auto& r = e.get_component<rendering::renderable_mesh_static>();
	r.is_enabled = true;
	arrow_transform.position = glm::vec3(-36.7 - _warning_selection * 0.3, 14 - _warning_selection * 0.3, -1.4);
	arrow_transform.scale = glm::vec3(0.8, 0.15,1);
	arrow_transform.is_matrix_dirty = true;

	// Change selected
	if (_input.is_input_started(core::controls::UP_CONTROL) || _input.is_input_started(core::controls::UP_CONTROL_PLAYER2))
	{
		if (_warning_selection == 0) {
			_warning_selection = 2;
		}
		_warning_selection--;
	}
	else if (_input.is_input_started(core::controls::DOWN_CONTROL) || _input.is_input_started(core::controls::DOWN_CONTROL_PLAYER2))
	{
		_warning_selection = (++_warning_selection) % 2;
	}
	// Choose selected
	else if (_input.is_input_started(core::controls::CARD1_CONTROL) || _input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2)) {

		if (_warning_selection == 0) {
			// Return to pause menu
			_seeing_new_menu = false;
			r.is_enabled = false;
			arrow_transform.position = glm::vec3(-35.5 - _selection * 0.4, 12 - _selection * 0.4, -5);
			arrow_transform.scale = glm::vec3(1, 0.2, 1);
			arrow_transform.is_matrix_dirty = true;
		}
		else if (_warning_selection == 1) {
			// Confirm quit
			_glfw.set_should_close(true);
		}
	}
}

void main_menu_controller::handle_main_menu_case(ecs::state& state, components::main_menu& menu, transforms::transform& arrow_transform)
{
	texts menu_items(state);
	auto half_height = _glfw.height() / 2.f;
	auto half_width = _glfw.width() / 2.f;

	// Move arrow
	if (_input.is_input_started(core::controls::UP_CONTROL) || _input.is_input_started(core::controls::UP_CONTROL_PLAYER2)) {
		if (_selection == 0) {
			_selection = NUM_CHOICES;
		}
		_selection--;
		arrow_transform.position = glm::vec3(-35.5 - _selection * 0.4, 12 - _selection * 0.4, -5);
		arrow_transform.is_matrix_dirty = true;
	}
	else if (_input.is_input_started(core::controls::DOWN_CONTROL) || _input.is_input_started(core::controls::DOWN_CONTROL_PLAYER2)) {
		_selection = (++_selection) % NUM_CHOICES;
		arrow_transform.position = glm::vec3(-35.5 - _selection * 0.4, 12 - _selection * 0.4, -5);
		arrow_transform.is_matrix_dirty = true;
	}

	// Choose selected
	else if (_input.is_input_started(core::controls::CARD1_CONTROL) || _input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2)) {
		if (_selection == START) {
			asset::scene_change_event game_start("assets/scenes/scene.json");
			_events.BroadcastEvent(game_start);
		}
		else if (_selection == HOW_TO) {
			menu.how_to_page++;
		}
		else if (_selection == OPTIONS) {
			_seeing_new_menu = true;
			_option_selection = 0;
		}
		else if (_selection == QUIT) {
			_seeing_new_menu = true;
			_warning_selection = 0;
		}
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