#include <engine-ui/developer_console.hpp>


#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::developer_console::developer_console(core::viewport& viewport, EventManager& events) : view(viewport), _events(events)
{
    setbuf(stdout, _outbuf);
    setbuf(stderr, _errbuf);
}

void engineui::developer_console::draw()
{
    auto buflen = strlen(_outbuf);
    auto outlen = strlen(_output);

    if (outlen + buflen > BufSize)
    {
        auto overflow = (outlen + buflen) - BufSize + 1;
        auto writelen = outlen - overflow;
        memcpy(_output, _output + overflow, writelen);
        outlen -= overflow;
    }

    memcpy(_output + outlen, _outbuf, buflen);

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
