#include <core/frame_timer.hpp>



#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "..\..\include\engine-ui\fps_display.hpp"



engineui::fps_display::fps_display(core::frame_timer& timer) : _frame_timer(timer)
{
}

void engineui::fps_display::draw()
{
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
    ImGui::Text(_frame_timer.frame_info().c_str());
    ImGui::End();
}
