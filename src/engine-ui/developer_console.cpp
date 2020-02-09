#include <engine-ui/developer_console.hpp>

#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::developer_console::developer_console(core::viewport& viewport, EventManager& events) : view(viewport), _events(events)
{
    setvbuf(stdout, _outbuf, _IOFBF, BUFSIZ);
    setvbuf(stderr, _errbuf, _IOFBF, BUFSIZ);
} 

void engineui::developer_console::draw()
{
    ImGui::SetNextWindowSize({ (float)_viewport.width, Height }, ImGuiCond_Always);
    ImGui::Begin("DevConsole");
       
    if (ImGui::IsWindowCollapsed())        
        ImGui::SetWindowPos({ 0, (float)_viewport.height - ImGui::GetWindowHeight() }, ImGuiCond_Always);
    else        
        ImGui::SetWindowPos({ 0, (float)_viewport.height - Height }, ImGuiCond_Always);

    static int output_type = 0;
    ImGui::RadioButton("command", &output_type, 0); ImGui::SameLine();
    ImGui::RadioButton("stdout", &output_type, 1); ImGui::SameLine();
    ImGui::RadioButton("stderr", &output_type, 2);

    ImGui::PushItemWidth(-1);

    switch (output_type)
    {
    case 0:        
        ImGui::InputTextMultiline("##cmd", _command, IM_ARRAYSIZE(_command), { 0, Height - 85 }, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputText("##in", _input, IM_ARRAYSIZE(_input));
        break;
    case 1:
        write_buffer(_output, _outbuf);
        ImGui::InputTextMultiline("##out", _output, IM_ARRAYSIZE(_output), { 0, Height - 60 }, ImGuiInputTextFlags_ReadOnly);
        break;
    case 2:
        write_buffer(_error, _errbuf);
        ImGui::InputTextMultiline("##err", _error, IM_ARRAYSIZE(_error), { 0, Height - 60 }, ImGuiInputTextFlags_ReadOnly);
        break;
    }
        
    ImGui::PopItemWidth();
    ImGui::End();
}

void engineui::developer_console::write_buffer(char* output, char* buffer)
{
    auto buflen = strlen(buffer);
    auto outlen = strlen(output);

    if (outlen + buflen > BufSize)
    {
        auto overflow = (outlen + buflen) - BufSize + 1;
        auto writelen = outlen - overflow;
        memcpy(output, output + overflow, writelen);
        outlen -= overflow;
    }

    memcpy(output + outlen, buffer, buflen);
}

char engineui::developer_console::_outbuf[BUFSIZ]{ "" };
char engineui::developer_console::_errbuf[BUFSIZ]{ "" };