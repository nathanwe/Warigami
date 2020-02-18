#include <engine-ui/entities_view.hpp>

#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::entities_view::entities_view(core::viewport& viewport, event::EventManager& events, ecs::state& r_ecs_state) : 
    view(viewport), _events(events), m_r_ecs_state(r_ecs_state), tree(m_r_ecs_state)
{
}

void engineui::entities_view::update(ecs::state state) {
    tree.clear();
    m_r_ecs_state.each_id< transforms::transform>([&](auto id, auto transform)
    {
        tree.insert(id);
    });
}

void engineui::entities_view::draw()
{
    // Draw tree
    ImGui::SetNextWindowSize({ Width, Height }, ImGuiCond_Once);
    ImGui::Begin("Entities");
    ImGui::SetWindowPos({ 10, 50 }, ImGuiCond_Once);

    ImGui::Unindent();

    tree.preorder([&]() {
        // On down
        ImGui::Indent();
    },
        [&](engineui::transform_tree::node node) {
        // On node
        ImGui::Bullet();
        ImGui::Button(std::to_string(node.id).c_str());
    },
        [&]() {
        // On up
        ImGui::Unindent();
    });

    ImGui::End();

    // Draw Inspector window
    ImGui::SetNextWindowSize({ InspectorWidth, InspectorHeight }, ImGuiCond_Once);
    ImGui::Begin("Inspector");
    ImGui::Text("Hello! I do nothing yet!");
    ImGui::SetWindowPos({ 10, Height+60 }, ImGuiCond_Once);
    ImGui::End();
}
