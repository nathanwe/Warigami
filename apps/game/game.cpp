#include <iostream>

#include <glbinding/glbinding.h>  // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include <core/frame_limiter.hpp>
#include <core/frame_timer.hpp>
#include <core/startup_config.hpp>
#include <core/system_info.hpp>
#include <core/input_manager.hpp>
#include <core/game_input_manager.hpp>

#include "sample_mgui.hpp"


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void run_game(GLFWwindow* window, uint32_t window_width, uint32_t window_height, bool is_debug);



int main() {

    core::startup_config conf;

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    std::string window_title_cpp = conf.window_title();
    const char* window_title_c = window_title_cpp.c_str();
    
    GLFWmonitor* p_monitor = nullptr;
    if (conf.fullscreen()) {
        p_monitor = glfwGetPrimaryMonitor();
    }
   
    GLFWwindow* window = glfwCreateWindow(conf.width(), conf.height(), window_title_c, p_monitor, nullptr);
    
    if (!conf.free_mouse()) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });

    run_game(window, conf.width(), conf.height(), true);
 
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void run_game(GLFWwindow* window, uint32_t window_width, uint32_t window_height, bool is_debug)
{

    core::system_info info;
    core::frame_timer timer;
    core::frame_limiter limiter(timer, 60);
    core::input_manager input(window);
    core::game_input_manager game_controls(input);

    sample_mgui mgui_sample(window);

    while (!glfwWindowShouldClose(window)) {
        timer.start();

        glfwPollEvents();
        input.update();
        game_controls.update();

        if (game_controls.is_input_active(core::controls::DESCEND_CONTROL)) std::cout << "Descending\n";
        if (game_controls.is_input_active(core::controls::ASCEND_CONTROL)) std::cout << "Ascending\n";
        if (game_controls.is_input_started(core::controls::INTERACT_CONTROL)) std::cout << "Interacted\n";

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mgui_sample.draw();

        glfwSwapBuffers(window);

        limiter.wait_remainder();
        timer.end();

        //std::cout << timer.frame_info() << std::endl;
    }

}