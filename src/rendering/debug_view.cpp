#include "rendering/debug_view.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace rendering
{
	debug_view::debug_view(core::viewport& viewport, renderer& renderer_) :
		view(viewport),
		_renderer(renderer_)
	{

	}

	void debug_view::draw()
	{
		ImGui::SetNextWindowPos({ 225, 50 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 200, 200 }, ImGuiCond_Once);
		ImGui::Begin("Debug Rendering Options");
		ImGui::Checkbox("Opaque Models", &_renderer._is_default_pass_enabled);
		ImGui::Checkbox("Collider Wireframes", &_renderer._is_debug_colliders);
		ImGui::Checkbox("Velocity Arrows", &_renderer._is_debug_velocity);
		ImGui::End();
	}
}