#include <engine-ui/developer_console.hpp>


#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::developer_console::developer_console(core::viewport& viewport, EventManager& events) : view(viewport), _events(events)
{

}

void engineui::developer_console::draw()
{
    ImGui::SetNextWindowSize({ (float)_viewport.width, Height }, ImGuiCond_Always);
    ImGui::Begin("DevConsole");
    if (ImGui::IsWindowCollapsed())        
        ImGui::SetWindowPos({ 0, (float)_viewport.height - ImGui::GetWindowHeight() }, ImGuiCond_Always);
    else        
        ImGui::SetWindowPos({ 0, (float)_viewport.height - Height }, ImGuiCond_Always);
    

    ImGui::PushItemWidth(-1);
    ImGui::InputTextMultiline("##out", _output, IM_ARRAYSIZE(_output), { 0, Height - 64 }, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputText("##in", _input, IM_ARRAYSIZE(_input));
    ImGui::PopItemWidth();
    ImGui::End();
}
