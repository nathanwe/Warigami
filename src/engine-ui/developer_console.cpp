#include <engine-ui/developer_console.hpp>


#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::developer_console::developer_console(EventManager& events) : _events(events)
{

}

void engineui::developer_console::draw()
{
    ImGui::SetNextWindowSize({ 600, 300 }, ImGuiCond_FirstUseEver);
    ImGui::Begin("DevConsole");
    ImGui::PushItemWidth(-1);
    ImGui::InputTextMultiline("##out", _output, IM_ARRAYSIZE(_output), { 0, 0 }, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputText("##in", _input, IM_ARRAYSIZE(_input));
    ImGui::PopItemWidth();

    ImGui::End();
}
