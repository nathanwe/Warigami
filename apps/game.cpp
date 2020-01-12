#include <GLFW/glfw3.h>
#include <iostream>

#include <glbinding/glbinding.h>


#include <core/frame_limiter.hpp>
#include <core/frame_timer.hpp>
#include <core/startup_config.hpp>
#include <core/system_info.hpp>
#include <core/input_manager.hpp>


int main() {
    
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glbinding::initialize(glfwGetProcAddress, true);

    /* Prevent framerate cap by gpu driver (don't wait for vblank before returning form glfwSwapBuffers) */
    glfwSwapInterval(0);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Sample Window", nullptr, nullptr);

    core::startup_config conf;
    core::system_info info;
    core::frame_timer timer;
    core::frame_limiter limiter(timer, 60);
    core::input_manager input(window);

  /*  if (!conf.free_mouse())
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);*/


    while (!glfwWindowShouldClose(window)) {
        timer.start();

        glfwPollEvents();

        limiter.wait_remainder();
        timer.end();
        std::cout << timer.frame_info() <<"f" << std::endl;
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}