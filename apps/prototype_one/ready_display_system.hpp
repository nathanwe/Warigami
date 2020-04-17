#ifndef __READY_DISPLAY_SYSTEM_HPP_
#define __READY_DISPLAY_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <core/glfw_context.hpp>

class ready_display_system : public ecs::system_base
{
	const glm::vec3 HidePosition = glm::vec3(0.f);

public:
	ready_display_system(core::glfw_context& glfw);

	void initialize(ecs::state& state);
	void update(ecs::state& state);

private:
	core::glfw_context& _glfw;
	glm::ivec2 _half_screen;

	ecs::entity* _board{ nullptr };
	ecs::entity* _players[2]{ nullptr, nullptr };
};

#endif