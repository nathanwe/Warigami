#include "rendering/renderer.hpp"

#include "ecs/state.hpp"
#include "rendering/camera.hpp"
#include "rendering/light_directional.hpp"
#include "rendering/light_point.hpp"
#include "rendering/renderable_mesh_static.hpp"
#include "transforms/transform.hpp"

#include "glbinding/gl/gl.h"
#include "glbinding/glbinding.h"
#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"

#include <iostream>

using namespace gl;

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

		_pass->bind(); // move to mesh only, move camera and lights to two ubos

		// Camera specific
		state.each<transforms::transform, camera>([&](auto& camera_transform, auto& cam)
		{
			if (cam.clear_setting == camera::clear_mode::flat_color)
			{
				glClearColor(cam.clear_color.x, cam.clear_color.y, cam.clear_color.z, 1.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			_pass->set_mat4(4, cam.view_projection);
			_pass->set_float3(18, camera_transform.position); // maybe need world position separately

			// Light directional specific
			state.each<transforms::transform, light_directional>([&](auto& light_transform, auto& light)
			{
				glm::vec3 light_inverse_direction = -1.f *
					glm::eulerAngleZXY(light_transform.rotation.z, light_transform.rotation.x, light_transform.rotation.y) * 
					glm::vec4(0, 0, -1, 1);

				_pass->set_float3(19, light_inverse_direction);
				_pass->set_float(20, light.intensity);
				_pass->set_float3(21, light.color);
			});

			// Light point specific
			int i = 0;
			state.each<transforms::transform, light_point>([&](auto& light_transform, auto& light)
			{
				if (i < 4) // should sort and use those closest to mesh
				{
					_pass->set_float3(22 + i, light_transform.position);
					_pass->set_float(26 + i, light.intensity);
					_pass->set_float3(30 + i, light.color);
					_pass->set_float(34 + i, light.radius);
					++i;
				}
			});

			// Mesh specific
			state.each<transforms::transform, renderable_mesh_static>([&](auto& transform, auto& renderable)
			{
				_pass->set_mat4(0, transform.local_to_world);

				_pass->set_texture(0, renderable.material.texture_diffuse);
				_pass->set_texture(1, renderable.material.texture_metalness);
				_pass->set_texture(2, renderable.material.texture_normal);
				_pass->set_texture(3, renderable.material.texture_roughness);
				_pass->set_texture(4, renderable.material.texture_ambient_occlusion);

				_pass->set_float2(8, renderable.material.texture_scale);
				_pass->set_float2(9, renderable.material.texture_offset);

				_pass->set_bool(10, renderable.material.texture_diffuse != 0);
				_pass->set_bool(11, renderable.material.texture_metalness != 0);
				_pass->set_bool(12, renderable.material.texture_normal != 0);
				_pass->set_bool(13, renderable.material.texture_roughness != 0);
				_pass->set_bool(14, renderable.material.texture_ambient_occlusion != 0);

				_pass->set_float3(15, renderable.material.param_diffuse);
				_pass->set_float(16, renderable.material.param_metalness);
				_pass->set_float(17, renderable.material.param_roughness);

				glBindVertexArray(renderable.mesh.vao);
				glDrawArrays(GL_TRIANGLES, 0, renderable.mesh.num_indices);

				_pass->set_texture(0, 0);
				_pass->set_texture(1, 0);
				_pass->set_texture(2, 0);
				_pass->set_texture(3, 0);
				_pass->set_texture(4, 0);
				glBindVertexArray(0);
			});
		});

		glfwSwapBuffers(_window);
	}
}
