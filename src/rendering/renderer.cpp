#include "rendering/renderer.hpp"

#include "collisions/collider.hpp"
#include "collisions/rigid_body.hpp"
#include "rendering/model.hpp"
#include "rendering/renderable_model_static.hpp"

#include <util/debounce.hpp>

#include "glbinding/gl/gl.h"
#include "glbinding/glbinding.h"
#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/quaternion.hpp"

#include <iostream>
#include <string>
#include <rendering/renderable_mesh_rigged.hpp>
#include <rendering/letterbox.hpp>

using namespace gl;

namespace rendering
{
    void draw_mesh_static(const mesh_static &mesh);

    void gl_debug_callback(
        GLenum source, 
        GLenum type, 
        GLuint id, 
        GLenum severity, 
        GLsizei length, 
        const GLchar *message,
        const void *user_parameter
    );

    renderer::renderer(
        core::viewport window_view, 
        bool is_debug, 
        asset_cache &cache,
        asset::asset_manager &assets, 
        const core::frame_timer &time,
        render_state& render_state,
        core::glfw_context& glfw)
        : _window_view(window_view)
        , _is_debug(is_debug)
        , _time(time)
        , _render_state(render_state)
        , _glfw(glfw)
    {
        initialize_backend();
        initialize_passes(assets);
        initialize_assets(cache);
        initialize_state(window_view);

        _glfw.set_window_changed_callback([&]() 
			{
                _window_view = letterbox({ 0, 0, _glfw.width(), _glfw.height() });
				glViewport(_window_view.x, _window_view.y, _window_view.width, _window_view.height);
            });
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

    void renderer::initialize_passes(asset::asset_manager &assets)
    {
        render_pass::description pass_default_desc (
            assets.get_proto_shader("assets/shaders/default.vert"),
            assets.get_proto_shader("assets/shaders/default.frag")
        );
        pass_default_desc.state.polygon_mode    = GL_FILL;
        pass_default_desc.state.uses_cull_face  = GL_TRUE;
        pass_default_desc.state.uses_depth_test = GL_TRUE;
        pass_default_desc.state.uses_blend      = GL_FALSE;
        pass_default_desc.state.culled_face     = GL_BACK;
        pass_default_desc.state.depth_func      = GL_LESS;
        pass_default_desc.state.depth_mask      = GL_TRUE;
        _pass_default = std::make_unique<render_pass>(pass_default_desc);
        assert(_pass_default);

        render_pass::description pass_animated_desc(
            assets.get_proto_shader("assets/shaders/animated.vert"),
            assets.get_proto_shader("assets/shaders/animated.frag")
        );
        pass_default_desc.state.polygon_mode    = GL_FILL;
        pass_default_desc.state.uses_cull_face  = GL_TRUE;
        pass_default_desc.state.uses_depth_test = GL_TRUE;
        pass_default_desc.state.uses_blend      = GL_FALSE;
        pass_default_desc.state.culled_face     = GL_BACK;
        pass_default_desc.state.depth_func      = GL_LESS;
        pass_default_desc.state.depth_mask      = GL_TRUE;
        _pass_animated = std::make_unique<render_pass>(pass_animated_desc);
		assert(_pass_animated);

		render_pass::description pass_blended_desc(
			assets.get_proto_shader("assets/shaders/default.vert"),
			assets.get_proto_shader("assets/shaders/blended.frag")
		);
		pass_blended_desc.state.polygon_mode    = GL_FILL;
		pass_blended_desc.state.uses_cull_face  = GL_TRUE;
		pass_blended_desc.state.uses_depth_test = GL_TRUE;
		pass_blended_desc.state.uses_blend      = GL_TRUE;
		pass_default_desc.state.culled_face     = GL_BACK;
		pass_blended_desc.state.blend_src       = GL_SRC_ALPHA;
		pass_blended_desc.state.blend_dest      = GL_ONE_MINUS_SRC_ALPHA;
		pass_blended_desc.state.depth_func      = GL_LEQUAL;
		pass_blended_desc.state.depth_mask      = GL_FALSE;
		_pass_blended = std::make_unique<render_pass>(pass_blended_desc);
		assert(_pass_blended);

        render_pass::description pass_cubemap_desc(
			assets.get_proto_shader("assets/shaders/cubemap.vert"),
			assets.get_proto_shader("assets/shaders/cubemap.frag")
		);
        pass_cubemap_desc.state.polygon_mode    = GL_FILL;
        pass_cubemap_desc.state.uses_cull_face  = GL_FALSE;
        pass_cubemap_desc.state.uses_depth_test = GL_TRUE;
        pass_cubemap_desc.state.uses_blend      = GL_FALSE;
        pass_cubemap_desc.state.depth_func      = GL_LEQUAL;
        pass_cubemap_desc.state.depth_mask      = GL_FALSE;
        _pass_cubemap = std::make_unique<render_pass>(pass_cubemap_desc);
        assert(_pass_cubemap);

        render_pass::description pass_debug_desc(
            assets.get_proto_shader("assets/shaders/debug.vert"),
            assets.get_proto_shader("assets/shaders/debug.frag")
        );
        pass_debug_desc.state.polygon_mode    = GL_LINE;
        pass_debug_desc.state.uses_cull_face  = GL_FALSE;
        pass_debug_desc.state.uses_depth_test = GL_TRUE;
        pass_debug_desc.state.uses_blend      = GL_FALSE;
        pass_debug_desc.state.depth_func      = GL_LESS;
        pass_debug_desc.state.depth_mask      = GL_TRUE;
        _pass_debug = std::make_unique<render_pass>(pass_debug_desc);
        assert(_pass_debug);
    }

    void renderer::initialize_assets(asset_cache &cache)
    {
        using namespace std::string_literals;
        _mesh_cube = cache.get<mesh_static>("assets/meshes/cube/cube.obj"s);
        assert(_mesh_cube.vao != 0 && _mesh_cube.num_indices != 0);
        _mesh_sphere = cache.get<mesh_static>("assets/meshes/sphere/sphere.obj"s);
        assert(_mesh_sphere.vao != 0 && _mesh_sphere.num_indices != 0);
        _mesh_arrow = cache.get<mesh_static>("assets/meshes/arrow/arrow.obj"s);
        assert(_mesh_arrow.vao != 0 && _mesh_arrow.num_indices != 0);
    }

    void renderer::initialize_state(core::viewport window_view)
    {
        _window_view = letterbox(window_view);
        glViewport(_window_view.x, _window_view.y, _window_view.width, _window_view.height);
        glClearColor(1, 0, 0, 1);
        glClearDepth(1);

        glBindFramebuffer(GL_FRAMEBUFFER, _render_state.target);
        glPolygonMode(GL_FRONT_AND_BACK, _render_state.polygon_mode);
        if (_render_state.uses_cull_face)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(_render_state.culled_face);
        } else
        {
            glDisable(GL_CULL_FACE);
        }
        if (_render_state.uses_depth_test)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(_render_state.depth_mask);
            gl::glDepthFunc(_render_state.depth_func);
        } else
        {
            glDisable(GL_DEPTH_TEST);
        }
        if (_render_state.uses_blend)
        {
            glEnable(GL_BLEND);
            gl::glBlendFunc(_render_state.blend_src, _render_state.blend_dest);
        } else
        {
            glDisable(GL_BLEND);
        }
    }

    void renderer::update(ecs::state &ecs_state)
    {
        transforms::transform *active_camera_transform = nullptr;
        camera *active_camera = nullptr;
        entity_id active_camera_id = 0;

        find_active_camera(ecs_state, active_camera_transform, active_camera, active_camera_id);
        if (active_camera == nullptr)
        {
            return;
        }

        clear_camera(*active_camera);
        if (_is_default_pass_enabled)
        {
            run_pass_default(ecs_state, *active_camera_transform, *active_camera);
            run_pass_animated(ecs_state, *active_camera_transform, *active_camera);
        }
        if (_is_debug_velocity)
        {
            run_pass_debug_velocity(ecs_state, *active_camera, active_camera_id);
        }
        if (_is_debug_colliders)
        {
            run_pass_debug_colliders(ecs_state, *active_camera);
		}
        if (active_camera->clear_setting == camera::clear_mode::sky_box)
        {
            run_pass_cubemap(*active_camera);
		}
		if (_is_default_pass_enabled)
		{
			run_pass_blended(ecs_state, *active_camera_transform, *active_camera);
		}
    }

    void renderer::find_active_camera(ecs::state &ecs_state, transforms::transform *&active_camera_transform,
                                      camera *&active_camera, entity_id &active_camera_id)
    {
        ecs_state.each_id<transforms::transform, camera>([&](auto id, auto &camera_transform, auto &cam) {
            active_camera_id = id;
            active_camera_transform = &camera_transform;
            active_camera = &cam;
        });
    }

    void renderer::clear_camera(const camera &cam)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDepthMask(GL_TRUE);

        switch (cam.clear_setting)
        {
            case camera::clear_mode::none:
                break;
            case camera::clear_mode::flat_color:
                glClearColor(cam.clear_color.x, cam.clear_color.y, cam.clear_color.z,
                             1.f); // have this as part of the render state and check if change is needed
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            case camera::clear_mode::sky_box:
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            default:
                assert(false);
        }
        glDepthMask(_render_state.depth_mask);
    }

    void
    renderer::run_pass_default(ecs::state &ecs_state, const transforms::transform& camera_transform, const camera &cam)
    {
        _pass_default->bind(_render_state);

		default_bind_camera(camera_transform, cam);

		_pass_default->set_float3(36, _ambient_light_color);
		_pass_default->set_float(37, _ambient_light_intensity);

        bool has_dir = false;
        ecs_state.each<transforms::transform, light_directional>(
            [&](auto &transform, auto &light) 
            {
                default_bind_light(transform, light);
                has_dir = true;
            });
        _pass_default->set_bool(38, has_dir);

        unsigned int i = 0;
        constexpr unsigned int max_num_lights = 4; // must match NUM_LIGHT_POINTS in "assets/shaders/default.frag"
        ecs_state.each<transforms::transform, light_point>([&](auto &transform, auto &light) {
            if (i < max_num_lights) // should sort and use those closest to mesh
            {
                default_bind_light(transform, light, i);
                ++i;
            }
        });
        _pass_default->set_int(34, i);

        ecs_state.each<transforms::transform, renderable_mesh_static>([&](auto &transform, auto &renderable) {
            if (renderable.is_enabled && !renderable.is_alpha_blended)
			{
				default_bind_renderable(transform, renderable.material);
				draw_mesh_static(renderable.mesh);
				default_unbind_renderable();
            }
        });

		ecs_state.each<transforms::transform, renderable_model_static>([&](auto& transform, auto& renderable)
		{
			for (auto& sub : renderable.model_.sub_models)
			{
				default_bind_sub_model(transform, sub);
				draw_mesh_static(sub.mesh);
				default_unbind_renderable();
			}
		});
	}
    void renderer::default_bind_camera(const transforms::transform &transform, const camera &cam)
    {
        _pass_default->set_mat4(4, cam.view_projection);
        _pass_default->set_float3(18, transform.position); // maybe need world position separately
    }

    void renderer::default_bind_light(const transforms::transform &transform, const light_directional &light)
    {
        glm::vec3 light_inverse_direction = -1.f *
                                            glm::eulerAngleZXY(transform.rotation.z, transform.rotation.x,
                                                               transform.rotation.y) *
                                            glm::vec4(0, 0, -1, 0);

        _pass_default->set_float3(19, light_inverse_direction);
        _pass_default->set_float(20, light.intensity);
        _pass_default->set_float3(21, light.color);
    }

    void renderer::default_bind_light(const transforms::transform &transform, const light_point &light, int i)
    {
        _pass_default->set_float3(22 + i, transform.position);
        _pass_default->set_float(26 + i, light.intensity);
        _pass_default->set_float3(30 + i, light.color);
    }

    void
    renderer::default_bind_renderable(const transforms::transform &transform, const material_pbr &material)
    {
        _pass_default->set_mat4(0, transform.local_to_world);

        _pass_default->set_texture(0, material.texture_diffuse);
        _pass_default->set_texture(1, material.texture_metalness);
        _pass_default->set_texture(2, material.texture_normal);
        _pass_default->set_texture(3, material.texture_roughness);
        _pass_default->set_texture(4, material.texture_ambient_occlusion);

        _pass_default->set_float2(8, material.texture_scale);
        _pass_default->set_float2(9, material.texture_offset);

        _pass_default->set_bool(10, material.texture_diffuse != 0);
        _pass_default->set_bool(11, material.texture_metalness != 0);
        _pass_default->set_bool(12, material.texture_normal != 0);
        _pass_default->set_bool(13, material.texture_roughness != 0);
        _pass_default->set_bool(14, material.texture_ambient_occlusion != 0);

        _pass_default->set_float3(15, material.param_diffuse);
        _pass_default->set_float(16, material.param_metalness);
        _pass_default->set_float(17, material.param_roughness);

        _pass_default->set_float3(35, material.tint_color);
    }

    void renderer::default_unbind_renderable()
    {
        _pass_default->set_texture(0, 0);
        _pass_default->set_texture(1, 0);
        _pass_default->set_texture(2, 0);
        _pass_default->set_texture(3, 0);
        _pass_default->set_texture(4, 0);
    }

	void renderer::run_pass_blended(ecs::state& ecs_state, const transforms::transform& camera_transform, const camera& cam)
	{
		_pass_blended->bind(_render_state);

		blended_bind_camera(camera_transform, cam);

		_pass_blended->set_float3(36, _ambient_light_color);
		_pass_blended->set_float(37, _ambient_light_intensity);

        bool has_dir = false; 
		ecs_state.each<transforms::transform, light_directional>(
            [&](auto& transform, auto& light) 
            {
			    blended_bind_light(transform, light);
                has_dir = true;
			});
        _pass_blended->set_bool(38, has_dir);

		unsigned int i = 0;
		constexpr unsigned int max_num_lights = 4; // must match NUM_LIGHT_POINTS in "assets/shaders/blended.frag"
		ecs_state.each<transforms::transform, light_point>(
            [&](auto& transform, auto& light) 
            {
			    if (i < max_num_lights) // should sort and use those closest to mesh
			    {
                    blended_bind_light(transform, light, i);
				    ++i;
			    }
			});
        _pass_blended->set_int(34, i);

        // sort meshes
        using blended_mesh = std::pair<const transforms::transform&, const renderable_mesh_static&>;
        std::map<float, std::vector<blended_mesh>> sorted_meshes;
		ecs_state.each<transforms::transform, renderable_mesh_static>(
			[&](auto& transform, auto& renderable)
			{
                if (renderable.is_enabled && renderable.is_alpha_blended)
				{
                    glm::vec3 camera_world_pos = camera_transform.local_to_world[3];
                    glm::vec3 sprite_world_pos = transform.local_to_world[3];
					float distSqr = glm::length2(camera_world_pos - sprite_world_pos);
                    sorted_meshes[distSqr].emplace_back(transform, renderable);
                }
			}
        );

		// bind meshes
        for (auto it = sorted_meshes.rbegin(); it != sorted_meshes.rend(); ++it)
		{
            for (auto& pair : it->second)
			{
				auto& transform = pair.first;
				auto& renderable = pair.second;

				blended_bind_renderable(transform, renderable.material);
				draw_mesh_static(renderable.mesh);
				blended_unbind_renderable();
            }
        }
	}

	void renderer::blended_bind_camera(const transforms::transform& transform, const camera& cam)
	{
		_pass_blended->set_mat4(4, cam.view_projection);
		_pass_blended->set_float3(18, transform.position);
	}

	void renderer::blended_bind_light(const transforms::transform& transform, const light_directional& light)
	{
		glm::vec3 light_inverse_direction = -1.f *
			glm::eulerAngleZXY(transform.rotation.z, transform.rotation.x,
				transform.rotation.y) *
			glm::vec4(0, 0, -1, 0);

		_pass_blended->set_float3(19, light_inverse_direction);
		_pass_blended->set_float(20, light.intensity);
		_pass_blended->set_float3(21, light.color);
	}

	void renderer::blended_bind_light(const transforms::transform& transform, const light_point& light, int i)
	{
		_pass_blended->set_float3(22 + i, transform.position);
		_pass_blended->set_float(26 + i, light.intensity);
		_pass_blended->set_float3(30 + i, light.color);
	}

	void renderer::blended_bind_renderable(const transforms::transform& transform, const material_pbr& material)
	{
		_pass_blended->set_mat4(0, transform.local_to_world);

		_pass_blended->set_texture(0, material.texture_diffuse);
		_pass_blended->set_texture(1, material.texture_metalness);
		_pass_blended->set_texture(2, material.texture_normal);
		_pass_blended->set_texture(3, material.texture_roughness);
		_pass_blended->set_texture(4, material.texture_ambient_occlusion);

		_pass_blended->set_float2(8, material.texture_scale);
		_pass_blended->set_float2(9, material.texture_offset);

		_pass_blended->set_bool(10, material.texture_diffuse != 0);
		_pass_blended->set_bool(11, material.texture_metalness != 0);
		_pass_blended->set_bool(12, material.texture_normal != 0);
		_pass_blended->set_bool(13, material.texture_roughness != 0);
		_pass_blended->set_bool(14, material.texture_ambient_occlusion != 0);

		_pass_blended->set_float3(15, material.param_diffuse);
		_pass_blended->set_float(16, material.param_metalness);
		_pass_blended->set_float(17, material.param_roughness);

		_pass_blended->set_float3(35, material.tint_color);
	}

	void renderer::blended_unbind_renderable()
	{
		_pass_blended->set_texture(0, 0);
		_pass_blended->set_texture(1, 0);
		_pass_blended->set_texture(2, 0);
		_pass_blended->set_texture(3, 0);
		_pass_blended->set_texture(4, 0);
	}

	void renderer::default_bind_sub_model(const transforms::transform& transform, const sub_model& sub)
	{
		_pass_default->set_mat4(0, transform.local_to_world);

		_pass_default->set_texture(0, sub.material.texture_diffuse);
		_pass_default->set_texture(1, sub.material.texture_metalness);
		_pass_default->set_texture(2, sub.material.texture_normal);
		_pass_default->set_texture(3, sub.material.texture_roughness);
		_pass_default->set_texture(4, sub.material.texture_ambient_occlusion);

		_pass_default->set_float2(8, sub.material.texture_scale);
		_pass_default->set_float2(9, sub.material.texture_offset);

		_pass_default->set_bool(10, sub.material.texture_diffuse != 0);
		_pass_default->set_bool(11, sub.material.texture_metalness != 0);
		_pass_default->set_bool(12, sub.material.texture_normal != 0);
		_pass_default->set_bool(13, sub.material.texture_roughness != 0);
		_pass_default->set_bool(14, sub.material.texture_ambient_occlusion != 0);

		_pass_default->set_float3(15, sub.material.param_diffuse);
		_pass_default->set_float(16, sub.material.param_metalness);
		_pass_default->set_float(17, sub.material.param_roughness);

		_pass_default->set_float3(35, sub.material.tint_color);
	}

    void renderer::run_pass_cubemap(const camera &cam)
    {
        _pass_cubemap->bind(_render_state);
        cubemap_bind_camera(cam);
        draw_mesh_static(_mesh_cube);
        cubemap_unbind();
    }

    void renderer::cubemap_bind_camera(const camera &cam)
    {
        assert(cam.clear_sky_box.id != 0);
        _pass_cubemap->set_cubemap(0, cam.clear_sky_box.id);

        glm::mat4 view_projection_no_translation = cam.projection * glm::mat4(glm::mat3(cam.view));
        _pass_cubemap->set_mat4(0, view_projection_no_translation);
    }

    void renderer::cubemap_unbind()
    {
        _pass_cubemap->set_cubemap(0, 0);
    }

    glm::mat4 local_to_parent(transforms::transform &transform, collisions::AABB_collider &collider)
    {
        glm::mat4 translation = glm::translate(glm::mat4(1), collider.position_absolute);
        glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3
                (
                        collider.left + collider.right,
                        collider.top + collider.bottom,
                        collider.front + collider.back
                ));
        return translation * scale;
    }

    glm::mat4 local_to_parent(transforms::transform &transform, collisions::sphere_collider &collider)
    {
        glm::mat4 translation = glm::translate(glm::mat4(1), collider.position_absolute);
        glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(collider.radius));
        return translation * scale;
    }

    void renderer::run_pass_debug_colliders(ecs::state &ecs_state, const camera &cam)
    {
        _pass_debug->bind(_render_state);
        _pass_debug->set_float3(4, _debug_collider_color);
        ecs_state.each<transforms::transform, collisions::AABB_collider>([&](auto &transform, auto &collider) {
            _pass_debug->set_mat4(0, cam.view_projection * local_to_parent(transform, collider));
            draw_mesh_static(_mesh_cube);
        });
        ecs_state.each<transforms::transform, collisions::sphere_collider>([&](auto &transform, auto &collider) {
            _pass_debug->set_mat4(0, cam.view_projection * local_to_parent(transform, collider));
            draw_mesh_static(_mesh_sphere);
        });
    }

    glm::mat4 debug_velocity_transform(const camera &cam, const transforms::transform &transform,
                                       const collisions::rigid_body &rigid_body)
    {
        glm::vec3 position = glm::vec3(transform.local_to_world * glm::vec4(0, 0, 0, 1));
        glm::mat4 translation = glm::translate(glm::mat4(1), position);

        glm::mat4 rotation = glm::toMat4(glm::quat(glm::vec3(0, 1, 0), rigid_body.velocity));

        const float garrett_nonsense = 0.5f;
        float speed = garrett_nonsense * glm::length(rigid_body.velocity);
        glm::vec3 vec_scale = glm::vec3(speed * 0.5f, speed, speed * 0.5f);
        glm::mat4 scale = glm::scale(glm::mat4(1), vec_scale);

        return cam.view_projection * translation * rotation * scale;
    }

    glm::mat4
    debug_cameras_velocity_transform(const camera &cam, const transforms::transform &transform, glm::vec3 &velocity)
    {
        glm::vec3 position = glm::vec3(transform.local_to_world * glm::vec4(0, 0, 0, 1));
        glm::mat4 translation = glm::translate(glm::mat4(1), position);

        glm::quat from_up_to_velocity = glm::quat(glm::vec3(0, 1, 0), velocity);
        glm::mat4 rotation = glm::toMat4(from_up_to_velocity);

        const float garrett_nonsense = 0.5f;
        float speed = garrett_nonsense * glm::length(velocity);
        speed = std::clamp(speed, 0.f, 0.5f);
        glm::vec3 vec_scale = glm::vec3(speed * 0.5f, speed, speed * 0.5f);
        glm::mat4 scale = glm::scale(glm::mat4(1), vec_scale);

        glm::mat4 camera_offset = glm::translate(glm::mat4(1), glm::vec3(0, 0.4f, -1));

        return cam.projection * camera_offset * cam.view * translation * rotation * scale;
    }

    void renderer::run_pass_debug_velocity(ecs::state &ecs_state, const camera &cam, entity_id active_camera_id)
    {
        _pass_debug->bind(_render_state);
        _pass_debug->set_float3(4, _debug_collider_color);
        ecs_state.each_id<transforms::transform, collisions::rigid_body>(
                [&](auto id, auto &transform, auto &rigid_body) {
                    glm::mat4 transformation = glm::mat4(1);
                    if (id != active_camera_id)
                    {
                        transformation = debug_velocity_transform(cam, transform, rigid_body);
                    } else
                    {
                        transformation = debug_cameras_velocity_transform(cam, transform, rigid_body.velocity);
                    }

                    _pass_debug->set_mat4(0, transformation);
                    draw_mesh_static(_mesh_arrow);
                });
    }

    void renderer::run_pass_animated(ecs::state &ecs_state, const transforms::transform& camera_transform, const camera &cam)
    {
        _pass_animated->bind(_render_state);

        default_bind_camera(camera_transform, cam);

        ecs_state.each<transforms::transform, light_directional>([&](auto &transform, auto &light) {
            default_bind_light(transform, light);
        });

        unsigned int i = 0;
        constexpr unsigned int max_num_lights = 4; // must match NUM_LIGHT_POINTS in "assets/shaders/default.frag"
        ecs_state.each<transforms::transform, light_point>([&](auto &transform, auto &light) {
            if (i < max_num_lights) // should sort and use those closest to mesh
            {
                default_bind_light(transform, light, i);
                ++i;
            }
        });

        ecs_state.each<transforms::transform, renderable_mesh_rigged>([&](auto &transform, auto &mesh_component) {
            default_bind_renderable(transform, mesh_component.material);
            bind_bones(mesh_component);
            draw_mesh_static(mesh_component.mesh);
            default_unbind_renderable();
        });
    }

    void renderer::bind_bones(renderable_mesh_rigged &component)
    {
        if (_bone_buffer.size() < component.bones.size())
            _bone_buffer.resize(component.bones.size());

        component.compute_transform(_bone_buffer.data());
        _pass_animated->set_mat4_array(45, _bone_buffer);
    }

    void draw_mesh_static(const mesh_static &mesh)
    {
        glBindVertexArray(mesh.vao);
        if (mesh.ebo)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
            glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        } else
        {
            glDrawArrays(GL_TRIANGLES, 0, mesh.num_indices);
        }
        glBindVertexArray(0);
    }

    void gl_debug_callback(
        GLenum source, 
        GLenum type, 
        GLuint id, 
        GLenum severity, 
        GLsizei length, 
        const GLchar *message,
        const void *user_parameter)
    {
        static auto print_msg = [](GLenum type, GLenum severity, const GLchar* message) {
            fprintf(stdout, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type, severity, message); 
        };

        static debounce<GLenum, GLenum, const GLchar *> print_debounce(float_second(1.f), print_msg);

        print_debounce(type, severity, message);        
        if (severity > GL_DEBUG_SEVERITY_MEDIUM) 
            print_msg(type, severity, message);
    }
}
