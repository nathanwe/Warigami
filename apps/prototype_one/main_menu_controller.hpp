#ifndef __MAIN_MENU_HPP_
#define __MAIN_MENU_HPP_

#include <glm/vec3.hpp>


#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/glfw_context.hpp>
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>


class main_menu_controller : public ecs::system_base
{
	const glm::vec3 HidePosition{ 9999.f, 0.f, 0.f };

public:
	main_menu_controller(
		event::EventManager& events, 
		core::glfw_context& glfw,
		core::game_input_manager& input,
		core::frame_timer& timer);

	void initialize(ecs::state& state) override;
	void update(ecs::state& state) override;

private:
	event::EventManager& _events;
	core::glfw_context& _glfw;
	core::game_input_manager& _input;
	core::frame_timer& _timer;
};


#endif