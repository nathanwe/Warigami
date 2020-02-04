#include <engine-ui/views/console.hpp>


#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


void engineui::view::console::on_command(std::function<void(std::string)> callback)
{
	_command_callback = callback;
}

void engineui::view::console::render()
{
    // some window
    ImGui::SetNextWindowPos({ 0, 0 }, 0, ImGui::GetWindowSize());
    ImGui::Begin("Developer Console");
    ImGui::Text("Hello, world %d", 123);  

    
    ImGui::End();
}