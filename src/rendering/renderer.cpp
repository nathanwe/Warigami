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
#include <string>

using namespace gl;

namespace rendering
{
	void draw_mesh_static(mesh_static& mesh);

	void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_parameter)
	{
		// reformat
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);

		//std::cout << "OpenGL Error Callback:\nSource: " << source << ", Type: " << type << ", Severity: " << severity << ", Length: " << length << ",\n    Message: " << message << std::endl;
	}

	renderer::renderer(GLFWwindow* window, viewport window_view, bool is_debug, asset_cache& cache) :
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

		render_pass::description pass_default_desc;
		pass_default_desc.filepath_vertex       = "assets/shaders/default.vert";
		pass_default_desc.filepath_fragment     = "assets/shaders/default.frag";
		pass_default_desc.state.uses_cull_face  = GL_TRUE;
		pass_default_desc.state.uses_depth_test = GL_TRUE;
		pass_default_desc.state.uses_blend      = GL_FALSE;
		pass_default_desc.state.culled_face     = GL_BACK;
		pass_default_desc.state.depth_func      = GL_LESS;
		pass_default_desc.state.depth_mask      = GL_TRUE;
		_pass_default = std::make_unique<render_pass>(pass_default_desc);
		assert(_pass_default.get());

		render_pass::description pass_cubemap_desc;
		pass_cubemap_desc.filepath_vertex       = "assets/shaders/cubemap.vert";
		pass_cubemap_desc.filepath_fragment     = "assets/shaders/cubemap.frag";
		pass_cubemap_desc.state.uses_cull_face  = GL_FALSE;
		pass_cubemap_desc.state.uses_depth_test = GL_TRUE;
		pass_cubemap_desc.state.uses_blend      = GL_FALSE;
		pass_cubemap_desc.state.depth_func      = GL_LEQUAL;
		pass_cubemap_desc.state.depth_mask      = GL_FALSE;
		_pass_cubemap = std::make_unique<render_pass>(pass_cubemap_desc);
		assert(_pass_cubemap.get());

		{
			using namespace std::string_literals;
			_mesh_cube = cache.get<mesh_static>("assets/meshes/cube/cube.obj"s);
			assert(_mesh_cube.vao != 0 && _mesh_cube.num_indices != 0);
		}

		glViewport(window_view.x, window_view.y, window_view.width, window_view.height);
		glClearColor(1, 0, 0, 1);
		glClearDepth(1);
	}

	void renderer::update(ecs::state& ecs_state)
	{
		// Camera specific
		ecs_state.each<transforms::transform, camera>([&](auto& camera_transform, auto& cam) // add some sort order field to camera component
		{
			// Have to rebind default framebuffer before clearing, likely because of double buffering
			_pass_default->bind(_render_state); // move to mesh only, move camera and lights to ubos

			switch (cam.clear_setting)
			{
			case camera::clear_mode::none:
				break;
			case camera::clear_mode::flat_color:
				glClearColor(cam.clear_color.x, cam.clear_color.y, cam.clear_color.z, 1.f); // have this as part of the render state and check if change is needed
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				break;
			case camera::clear_mode::sky_box:
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				break;
			default:
				assert(false);
			}

			_pass_default->set_mat4(4, cam.view_projection);
			_pass_default->set_float3(18, camera_transform.position); // maybe need world position separately

			// Light directional specific
			ecs_state.each<transforms::transform, light_directional>([&](auto& light_transform, auto& light)
			{
				glm::vec3 light_inverse_direction = -1.f *
					glm::eulerAngleZXY(light_transform.rotation.z, light_transform.rotation.x, light_transform.rotation.y) * 
					glm::vec4(0, 0, -1, 1);

				_pass_default->set_float3(19, light_inverse_direction);
				_pass_default->set_float(20, light.intensity);
				_pass_default->set_float3(21, light.color);
			});

			// Light point specific
			unsigned int i = 0;
			constexpr unsigned int max_num_lights = 4; // must match NUM_LIGHT_POINTS in "assets/shaders/default.frag"
			ecs_state.each<transforms::transform, light_point>([&](auto& light_transform, auto& light)
			{
				if (i < max_num_lights) // should sort and use those closest to mesh
				{
					_pass_default->set_float3(22 + i, light_transform.position);
					_pass_default->set_float(26 + i, light.intensity);
					_pass_default->set_float3(30 + i, light.color);
					++i;
				}
			});

			// Mesh specific
			ecs_state.each<transforms::transform, renderable_mesh_static>([&](auto& transform, auto& renderable)
			{
				_pass_default->set_mat4(0, transform.local_to_world);

				_pass_default->set_texture(0, renderable.material.texture_diffuse);
				_pass_default->set_texture(1, renderable.material.texture_metalness);
				_pass_default->set_texture(2, renderable.material.texture_normal);
				_pass_default->set_texture(3, renderable.material.texture_roughness);
				_pass_default->set_texture(4, renderable.material.texture_ambient_occlusion);

				_pass_default->set_float2(8, renderable.material.texture_scale);
				_pass_default->set_float2(9, renderable.material.texture_offset);

				_pass_default->set_bool(10, renderable.material.texture_diffuse != 0);
				_pass_default->set_bool(11, renderable.material.texture_metalness != 0);
				_pass_default->set_bool(12, renderable.material.texture_normal != 0);
				_pass_default->set_bool(13, renderable.material.texture_roughness != 0);
				_pass_default->set_bool(14, renderable.material.texture_ambient_occlusion != 0);

				_pass_default->set_float3(15, renderable.material.param_diffuse);
				_pass_default->set_float(16, renderable.material.param_metalness);
				_pass_default->set_float(17, renderable.material.param_roughness);

				draw_mesh_static(renderable.mesh);

				_pass_default->set_texture(0, 0);
				_pass_default->set_texture(1, 0);
				_pass_default->set_texture(2, 0);
				_pass_default->set_texture(3, 0);
				_pass_default->set_texture(4, 0);
			});

			/**/
			if (cam.clear_setting == camera::clear_mode::sky_box)
			{
				_pass_cubemap->bind(_render_state);

				_pass_cubemap->set_cubemap(0, cam.clear_sky_box.id);

				glm::mat4 view_projection_no_translation = cam.projection * glm::mat4(glm::mat3(cam.view));
				_pass_cubemap->set_mat4(0, view_projection_no_translation);

				draw_mesh_static(_mesh_cube);

				_pass_cubemap->set_cubemap(0, 0);
			}
			/**/
		});

		glfwSwapBuffers(_window);
	}

	void draw_mesh_static(mesh_static& mesh)
	{
		glBindVertexArray(mesh.vao);
		if (mesh.ebo)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
			glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, mesh.num_indices);
		}
		glBindVertexArray(0);
	}
}
