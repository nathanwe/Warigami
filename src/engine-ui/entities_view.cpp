#include <engine-ui/entities_view.hpp>

#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::entities_view::entities_view(core::viewport& viewport, EventManager& events) : view(viewport), _events(events)
{
}

void engineui::entities_view::update(ecs::state state) {
    if (_entities.size() > 0) {
        _entities.clear();
    }
    //// Get all entities and sort them
    state.each< transforms::transform>([&](transforms::transform& transform)
    {
        _entities.push_front(transform.parent);
    });
    _entities.sort();
}

void engineui::entities_view::draw()
{

    ImGui::SetNextWindowSize({ Width, Height }, ImGuiCond_Always);
    ImGui::Begin("Entities");
    ImGui::SetWindowPos({ 0, 50 }, ImGuiCond_Always);

    // For each entity
    for (std::list<entity_id>::iterator iter = _entities.begin(); iter != _entities.end(); iter++) {
        // Make a tree node
        if (ImGui::TreeNode(std::to_string(*iter).c_str()))
        {
            ImGui::Indent();
            ImGui::Text("dummy");
            ImGui::Unindent();
            ImGui::TreePop();
        }
    }
    ImGui::End();
}