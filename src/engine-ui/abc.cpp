#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ecs/ecs_types.hpp"

#include <vector>

engineui::fps_display::fps_display(core::viewport& viewport, ecs::state& r_ecs_state) : view(viewport), m_r_ecs_state(r_ecs_state)
{
}

class node
{
	ecs::entity_id id;
	std::vector<entity_id> children;
};

node* find(std::vector<node>& roots, entity_id id)
{
	
}

void engineui::fps_display::draw()
{
	std::vector<node> roots;

	m_r_ecs_state.each_id<transform>([&](auto id, auto& t) 
	{
		node self;
		self.id = id;
		
	});
}
