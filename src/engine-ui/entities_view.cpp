#include <engine-ui/entities_view.hpp>

#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::entities_view::entities_view(core::viewport& viewport, EventManager& events, ecs::state& r_ecs_state) : 
    view(viewport), _events(events), m_r_ecs_state(r_ecs_state), tree(m_r_ecs_state)
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
        // Get entity name and id
        const char* name = "Temp name";
        // Make a tree node
        if (ImGui::TreeNode(name))
        {
            ImGui::Indent();
            ImGui::Text("dummy");
            ImGui::Unindent();
            ImGui::TreePop();
        }
    }
    ImGui::End();
}
