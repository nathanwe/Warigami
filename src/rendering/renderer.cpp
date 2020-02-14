#include "rendering/renderer.hpp"

#include "collisions/collider.hpp"

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
		initialize_backend();
		initialize_passes();
		initialize_assets(cache);
		initialize_state(window_view);
	}

	void renderer::initialize_backend()
	{
		glbinding::initialize(glfwGetProcAddress, true);

		if (_is_debug)
		{
			std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(gl_debug_callback, nullptr);
		}
	}

	void renderer::initialize_passes()
	{
		render_pass::description pass_default_desc;
		pass_default_desc.filepath_vertex = "assets/shaders/default.vert";
		pass_default_desc.filepath_fragment = "assets/shaders/default.frag";
		pass_default_desc.state.polygon_mode = GL_FILL;
		pass_default_desc.state.uses_cull_face = GL_TRUE;
		pass_default_desc.state.uses_depth_test = GL_TRUE;
		pass_default_desc.state.uses_blend = GL_FALSE;
		pass_default_desc.state.culled_face = GL_BACK;
		pass_default_desc.state.depth_func = GL_LESS;
		pass_default_desc.state.depth_mask = GL_TRUE;
		_pass_default = std::make_unique<render_pass>(pass_default_desc);
		assert(_pass_default.get());

		render_pass::description pass_cubemap_desc;
		pass_cubemap_desc.filepath_vertex = "assets/shaders/cubemap.vert";
		pass_cubemap_desc.filepath_fragment = "assets/shaders/cubemap.frag";
		pass_cubemap_desc.state.polygon_mode = GL_FILL;
		pass_cubemap_desc.state.uses_cull_face = GL_FALSE;
		pass_cubemap_desc.state.uses_depth_test = GL_TRUE;
		pass_cubemap_desc.state.uses_blend = GL_FALSE;
		pass_cubemap_desc.state.depth_func = GL_LEQUAL;
		pass_cubemap_desc.state.depth_mask = GL_FALSE;
		_pass_cubemap = std::make_unique<render_pass>(pass_cubemap_desc);
		assert(_pass_cubemap.get());

		render_pass::description pass_debug_desc;
		pass_debug_desc.filepath_vertex = "assets/shaders/debug.vert";
		pass_debug_desc.filepath_fragment = "assets/shaders/debug.frag";
		pass_debug_desc.state.polygon_mode = GL_LINE;
		pass_debug_desc.state.uses_cull_face = GL_FALSE;
		pass_debug_desc.state.uses_depth_test = GL_TRUE;
		pass_debug_desc.state.uses_blend = GL_FALSE;
		pass_debug_desc.state.depth_func = GL_LESS;
		pass_debug_desc.state.depth_mask = GL_FALSE;
		_pass_debug = std::make_unique<render_pass>(pass_debug_desc);
		assert(_pass_debug.get());
	}

	void renderer::initialize_assets(asset_cache& cache)
	{
		using namespace std::string_literals;
		_mesh_cube = cache.get<mesh_static>("assets/meshes/cube/cube.obj"s);
		assert(_mesh_cube.vao != 0 && _mesh_cube.num_indices != 0);
		_mesh_sphere = cache.get<mesh_static>("assets/meshes/sphere/sphere.obj"s);
		assert(_mesh_sphere.vao != 0 && _mesh_sphere.num_indices != 0);
	}

	void renderer::initialize_state(viewport window_view)
	{
		glViewport(window_view.x, window_view.y, window_view.width, window_view.height);
		glClearColor(1, 0, 0, 1);
		glClearDepth(1);
	}

	void renderer::update(ecs::state& ecs_state)
	{
		transforms::transform* active_camera_transform = nullptr;
		camera* active_camera;

		find_active_camera(ecs_state, active_camera_transform, active_camera);
		if (active_camera == nullptr)
		{
			return;
		}

		clear_camera(*active_camera);

		run_pass_default(ecs_state, *active_camera_transform, *active_camera);

		if (_is_debug_colliders)
		{
			run_pass_debug_colliders(ecs_state, *active_camera);
		}

		if (active_camera->clear_setting == camera::clear_mode::sky_box)
		{
			run_pass_cubemap(*active_camera);
		}

		glfwSwapBuffers(_window);
	}

	void renderer::find_active_camera(ecs::state& ecs_state, transforms::transform*& camera_transform, camera*& cam)
	{
		// 
		ecs_state.each<transforms::transform, camera>([&](auto& camera_transform, auto& cam)
		{
			active_camera_transform = &camera_transform;
			active_camera = &cam;
		});
	}

	void renderer::clear_camera(const camera& cam)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	}

	void renderer::run_pass_default(ecs::state& ecs_state, const transforms::transform camera_transform, const camera& cam)
	{
		_pass_default->bind(_render_state);

		default_bind_camera(camera_transform, cam);

		ecs_state.each<transforms::transform, light_directional>([&](auto& transform, auto& light)
		{
			default_bind_light(transform, light);
		});

		unsigned int i = 0;
		constexpr unsigned int max_num_lights = 4; // must match NUM_LIGHT_POINTS in "assets/shaders/default.frag"
		ecs_state.each<transforms::transform, light_point>([&](auto& transform, auto& light)
		{
			if (i < max_num_lights) // should sort and use those closest to mesh
			{
				default_bind_light(transform, light, i);
				++i;
			}
		});

		ecs_state.each<transforms::transform, renderable_mesh_static>([&](auto& transform, auto& renderable)
		{
			default_bind_renderable(transform, renderable);
			draw_mesh_static(renderable.mesh);
			default_unbind_renderable();
		});
	}

	void renderer::default_bind_camera(const transforms::transform& transform, const camera& cam)
	{
		_pass_default->set_mat4(4, cam.view_projection);
		_pass_default->set_float3(18, transform.position); // maybe need world position separately
	}

	void renderer::default_bind_light(const transforms::transform& transform, const light_directional& light)
	{
		glm::vec3 light_inverse_direction = -1.f *
			glm::eulerAngleZXY(transform.rotation.z, transform.rotation.x, transform.rotation.y) *
			glm::vec4(0, 0, -1, 1);

		_pass_default->set_float3(19, light_inverse_direction);
		_pass_default->set_float(20, light.intensity);
		_pass_default->set_float3(21, light.color);
	}

	void renderer::default_bind_light(const transforms::transform& transform, const light_point& light, int i)
	{
		_pass_default->set_float3(22 + i, transform.position);
		_pass_default->set_float(26 + i, light.intensity);
		_pass_default->set_float3(30 + i, light.color);
	}

	void renderer::default_bind_renderable(const transforms::transform& transform, const renderable_mesh_static& renderable)
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
	}

	void renderer::default_unbind_renderable()
	{
		_pass_default->set_texture(0, 0);
		_pass_default->set_texture(1, 0);
		_pass_default->set_texture(2, 0);
		_pass_default->set_texture(3, 0);
		_pass_default->set_texture(4, 0);
	}

	void renderer::run_pass_cubemap(const camera& cam)
	{
		cubemap_bind_camera(cam);
		draw_mesh_static(_mesh_cube);
		cubemap_unbind();
	}

	void renderer::cubemap_bind_camera(const camera& cam)
	{
		assert(cam.clear_sky_box.id != 0);
		_pass_cubemap->bind(_render_state);
		_pass_cubemap->set_cubemap(0, cam.clear_sky_box.id);

		glm::mat4 view_projection_no_translation = cam.projection * glm::mat4(glm::mat3(cam.view));
		_pass_cubemap->set_mat4(0, view_projection_no_translation);
	}

	void renderer::cubemap_unbind()
	{
		_pass_cubemap->set_cubemap(0, 0);
	}

	void renderer::run_pass_debug_colliders(ecs::state& ecs_state, const camera& cam)
	{
		_pass_debug->bind(_render_state);
		_pass_debug->set_float3(4, _debug_collider_color);
		ecs_state.each<transforms::transform, collisions::AABB_collider>([&](auto& transform, auto& collider)
		{
			_pass_debug->set_mat4(0, cam.view_projection * transform.local_to_world);
			draw_mesh_static(_mesh_cube);
		});
		ecs_state.each<transforms::transform, collisions::sphere_collider>([&](auto& transform, auto& collider)
		{
			_pass_debug->set_mat4(0, cam.view_projection * transform.local_to_world);
			draw_mesh_static(_mesh_sphere);
		});
	}

	void draw_mesh_static(const mesh_static& mesh)
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
