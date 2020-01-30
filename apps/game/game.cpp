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

#include "sample_mgui.hpp"


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}



int main() {

    core::startup_config conf;

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    GLFWwindow* window = glfwCreateWindow(conf.height(), conf.width(), "Sample Window", nullptr, nullptr);
    //GLFWwindow* window = glfwCreateWindow(600, 800, "Sample Window", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });

    
    core::system_info info;
    core::frame_timer timer;
    core::frame_limiter limiter(timer, 60);
    core::input_manager input(window);

    sample_mgui mgui_sample(window);

    while (!glfwWindowShouldClose(window)) {
        timer.start();

        glfwPollEvents();
        input.update();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        mgui_sample.draw();

        glfwSwapBuffers(window);

        limiter.wait_remainder();
        timer.end();

        std::cout << timer.frame_info() << std::endl;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}