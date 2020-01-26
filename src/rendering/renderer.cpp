#include "rendering/renderer.hpp"

#include "ecs/state.hpp"
#include "rendering/camera.hpp"
#include "rendering/renderable_mesh_static.hpp"
#include "transforms/transform.hpp"

#include "glbinding/gl/gl.h"
#include "glbinding/glbinding.h"

using namespace gl;

#include <iostream>

namespace rendering
{
	void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_parameter)
	{
		// reformat
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);

		//std::cout << "OpenGL Error Callback:\nSource: " << source << ", Type: " << type << ", Severity: " << severity << ", Length: " << length << ",\n    Message: " << message << std::endl;
	}

	renderer::renderer(GLFWwindow* window, viewport window_view, bool is_debug) :
		_window(window),
		_window_view(window_view),
		_is_debug(is_debug)
	{
		glbinding::initialize(glfwGetProcAddress, true);

		if (_is_debug)
		{
			std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(gl_debug_callback, nullptr);
		}

		_pass = std::make_unique<render_pass>("assets/shaders/default.vert", "assets/shaders/default.frag");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(window_view.x, window_view.y, window_view.width, window_view.height);
		glClearColor(0, 0.5f, 0.7f, 1.f);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
	}

	void renderer::update(ecs::state& state)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		state.each<camera>([&](auto& camera) 
		{
			_pass->bind();
			state.each<transforms::transform, renderable_mesh_static>([&](auto& transform, auto& renderable)
			{
				glm::mat4 world_view_projection = camera.view_projection * transform.local_to_world;
				_pass->set_mat4(0, world_view_projection);

				glBindVertexArray(renderable.mesh.vao);
				glDrawArrays(GL_TRIANGLES, 0, renderable.mesh.num_indices);
			});
		});

		glfwSwapBuffers(_window);
	}
}
