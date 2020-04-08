#ifndef __MAIN_MENU_HPP_
#define __MAIN_MENU_HPP_

#include <glm/vec3.hpp>
#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/glfw_context.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>

#include "components/main_menu.hpp"


class main_menu_controller : public ecs::system_base
{
	const glm::vec3 HidePosition{ 9999.f, 0.f, 0.f };

	struct texts
	{
		texts(ecs::state& state)
			: start_text(state.find_entity(101).get_component<rendering::renderable_text>())
			, howto_text(state.find_entity(102).get_component<rendering::renderable_text>())
			, options_text(state.find_entity(103).get_component<rendering::renderable_text>())
			, exit_text(state.find_entity(108).get_component<rendering::renderable_text>())
		{}

		rendering::renderable_text& start_text;
		rendering::renderable_text& howto_text;
		rendering::renderable_text& options_text;
		rendering::renderable_text& exit_text;
	};

public:
	main_menu_controller(
		event::EventManager& events, 
		core::glfw_context& glfw,
		core::game_input_manager& input,
		core::frame_timer& timer);

	void initialize(ecs::state& state) override;
	void update(ecs::state& state) override;

	void handle_howto_case(
		ecs::state& state,
		components::main_menu& menu);

	void handle_main_menu_case(
		ecs::state& state,
		components::main_menu& menu);

private:
	event::EventManager& _events;
	core::glfw_context& _glfw;
	core::game_input_manager& _input;
	core::frame_timer& _timer;
};


#endif