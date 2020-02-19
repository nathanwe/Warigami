#include <engine-ui/entities_view.hpp>

#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::entities_view::entities_view(core::viewport& viewport, event::EventManager& events, ecs::state& r_ecs_state) :
        view(viewport), _events(events), m_r_ecs_state(r_ecs_state), tree(m_r_ecs_state), _current_entity_selected(nullptr)
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
                      if (ImGui::Button(std::to_string(node.id).c_str())) {
                          _current_entity_selected = &(m_r_ecs_state.find_entity(node.id));
                      }
                  },
                  [&]() {
                      // On up
                      ImGui::Unindent();
                  });

    ImGui::End();

    // Draw Inspector window
    ImGui::SetNextWindowSize({ InspectorWidth, InspectorHeight }, ImGuiCond_Once);
    ImGui::Begin("Inspector");
    ImGui::SetWindowPos({ 10, Height+60 }, ImGuiCond_Once);

    if (_current_entity_selected == nullptr) {
        ImGui::Text("Select an entity");
    }
    else {
        ImGui::Text("Entity %s", std::to_string(_current_entity_selected->id()).c_str());
        ImGui::Indent();
        auto archetype = _current_entity_selected->archetype_id();
        ImGui::Text("Archetype id: %llu", archetype);
        /*ImGui::Text("Components:");
        ImGui::Indent();*/
        ImGui::Text("Transform:");
        transforms::transform* tr = &_current_entity_selected->get_component<transforms::transform>();
        if (tr != nullptr) {
            ImGui::Indent();
            ImGui::Text("x: %.2f", tr->position.x);
            ImGui::Text("y: %.2f", tr->position.y);
            ImGui::Text("z: %.2f", tr->position.z);
            ImGui::Text("rotation x: %.2f", tr->rotation.x);
            ImGui::Text("rotation y: %.2f", tr->rotation.y);
            ImGui::Text("rotation z: %.2f", tr->rotation.z);
            ImGui::Text("scale x: %.2f", tr->scale.x);
            ImGui::Text("scale y: %.2f", tr->scale.y);
            ImGui::Text("scale z: %.2f", tr->scale.z);
            ImGui::Unindent();
        }
        //ImGui::Unindent();
    }

    ImGui::Unindent();
    ImGui::End();
}
