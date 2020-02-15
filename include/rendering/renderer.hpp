#ifndef WIZARDPEOPLE_RENDERING_RENDERER_HPP
#define WIZARDPEOPLE_RENDERING_RENDERER_HPP

#include "ecs/state.hpp"
#include "ecs/system_base.hpp"
#include "rendering/asset_cache.hpp"
#include "rendering/camera.hpp"
#include "rendering/light_directional.hpp"
#include "rendering/light_point.hpp"
#include "rendering/mesh_static.hpp"
#include "rendering/renderable_mesh_static.hpp"
#include "rendering/render_pass.hpp"
#include "rendering/render_state.hpp"
#include "transforms/transform.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <memory>

namespace rendering
{
	struct viewport
	{
		uint32_t x, y, width, height;
	};

	class renderer : public ecs::system_base
	{
	public:
		renderer(GLFWwindow* window, viewport window_view, bool is_debug, asset_cache& cache);

	public:
		virtual void update(ecs::state& state) override;

	private:
		void initialize_backend();
		void initialize_passes();
		void initialize_assets(asset_cache& cache);
		void initialize_state(viewport window_view);

		void find_active_camera(ecs::state& ecs_state, transforms::transform*& camera_transform, camera*& cam);

		void run_pass_default(ecs::state& ecs_state, const transforms::transform camera_transform, const camera& cam);
		void clear_camera(const camera& cam);
		void default_bind_camera(const transforms::transform& transform, const camera& cam);
		void default_bind_light(const transforms::transform& transform, const light_directional& light);
		void default_bind_light(const transforms::transform& transform, const light_point& light, int i);
		void default_bind_renderable(const transforms::transform& transform, const renderable_mesh_static& renderable);
		void default_unbind_renderable();

		void run_pass_cubemap(const camera& cam);
		void cubemap_bind_camera(const camera& cam);
		void cubemap_unbind();

		void run_pass_debug_colliders(ecs::state& ecs_state, const camera& cam);

	private:
		bool _is_debug = false;
		GLFWwindow* _window = nullptr;
		viewport _window_view;
		std::unique_ptr<render_pass> _pass_default;
		std::unique_ptr<render_pass> _pass_cubemap;
		std::unique_ptr<render_pass> _pass_debug;
		mesh_static _mesh_cube;
		mesh_static _mesh_sphere;
		render_state _render_state;
		bool _is_debug_colliders = false; 
		bool _is_debug_velocity = false;
		glm::vec3 _debug_collider_color = glm::vec3(0, 1, 0);
	};
}

#endif
