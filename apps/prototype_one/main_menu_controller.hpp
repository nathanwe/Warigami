#ifndef __MAIN_MENU_HPP_
#define __MAIN_MENU_HPP_

#include <glm/vec3.hpp>
#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/glfw_context.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>

#include "components/main_menu.hpp"
#include <transforms/transform.hpp>


class main_menu_controller : public ecs::system_base
{
	const glm::vec3 HidePosition{ 9999.f, 0.f, 0.f };

	enum choices { START, HOW_TO, OPTIONS, QUIT, NUM_CHOICES };
	enum options { FULLSCREEN, MUTE_ALL, MUTE_MUSIC, BACK, NUM_OPTIONS };

public:
	main_menu_controller(
		event::EventManager& events, 
		core::glfw_context& glfw,
		core::game_input_manager& input,
		core::frame_timer& timer,
		core::startup_config& config);

	void initialize(ecs::state& state) override;
	void update(ecs::state& state) override;

	void handle_howto_case(
		ecs::state& state,
		components::main_menu& menu);

	void handle_main_menu_case(
		ecs::state& state,
		components::main_menu& menu,
		transforms::transform& arrow_transform);

	void handle_options_case(
		ecs::state& state,
		components::main_menu& menu,
		transforms::transform& arrow_transform);

	void handle_quit_case(
		ecs::state& state,
		components::main_menu& menu,
		transforms::transform& arrow_transform);

private:
	event::EventManager& _events;
	core::glfw_context& _glfw;
	core::game_input_manager& _input;
	core::frame_timer& _timer;
	core::startup_config& _config;

	int _selection;
	bool _seeing_new_menu;
	int _option_selection;
	int _warning_selection;
};


#endif