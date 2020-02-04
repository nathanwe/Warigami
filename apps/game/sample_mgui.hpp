#ifndef __SAMPLE_MGUI_HPP_
#define __SAMPLE_MGUI_HPP_

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void MySaveFunction()
{
    std::cout << "Button click." << std::endl;
}

// this is a mgui test that should be deleted and never used
class sample_mgui
{
    const char* glsl_version = "#version 420";


public:
	sample_mgui(GLFWwindow* window) : _window(window)
	{
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
	}

    void draw()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // some window
        auto sz = ImGui::GetWindowSize();
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize(ImGui::GetWindowSize());

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        ImGui::Text("Hello, world %d", 123);
        if (ImGui::Button("Save"))
            MySaveFunction();
                
        ImGui::InputText("string", some_string, IM_ARRAYSIZE(some_string));
        ImGui::SliderFloat("float", &some_float, 0.0f, 1.0f);
        ImGui::End();


        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    GLFWwindow* _window;

    float some_float{ 1.f };
    char some_string[1024] { "foo" };
};

#endif // !__SAMPLE_MGUI_HPP_
