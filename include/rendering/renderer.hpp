#ifndef WIZARDPEOPLE_RENDERING_RENDERER_HPP
#define WIZARDPEOPLE_RENDERING_RENDERER_HPP

#include "asset/asset_manager.hpp"
#include "core/frame_timer.hpp"
#include "core/viewport.hpp"
#include "core/glfw_context.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/state.hpp"
#include "ecs/system_base.hpp"
#include "rendering/asset_cache.hpp"
#include "rendering/camera.hpp"
#include "rendering/light_directional.hpp"
#include "rendering/light_point.hpp"
#include "rendering/mesh_static.hpp"
#include "rendering/renderable_mesh_static.hpp"
#include "rendering/renderable_model_static.hpp"
#include "rendering/renderable_mesh_rigged.hpp"
#include "rendering/render_pass.hpp"
#include "rendering/render_state.hpp"
#include "transforms/transform.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <memory>

namespace rendering
{
	

	class renderer : public ecs::system_base
	{
	public:
		renderer(
			core::viewport window_view, 
			bool is_debug, 
			asset_cache& cache, 
			asset::asset_manager& assets, 
			const core::frame_timer& time,
			render_state& render_state,
			core::glfw_context& glfw);

	public:
		void update(ecs::state& state) override;

	public:
		bool _is_default_pass_enabled = true;
		bool _is_debug_colliders = false;
		bool _is_debug_velocity = false;

	private:
		void initialize_backend();
		void initialize_passes(asset::asset_manager& assets);
		void initialize_assets(asset_cache& cache);
		void initialize_state(core::viewport window_view);

		void find_active_camera(ecs::state& ecs_state, transforms::transform*& camera_transform, camera*& cam, entity_id& active_camera_id);

		void run_pass_default(ecs::state& ecs_state, const transforms::transform& camera_transform, const camera& cam);
		void clear_camera(const camera& cam);
		void default_bind_camera(const transforms::transform& transform, const camera& cam);
		void default_bind_light(const transforms::transform& transform, const light_directional& light);
		void default_bind_light(const transforms::transform& transform, const light_point& light, int i);
		void default_bind_renderable(const transforms::transform& transform, const material_pbr& material);
		void default_bind_sub_model(const transforms::transform& transform, const sub_model& sub);
		void default_unbind_renderable();

		void run_pass_blended(ecs::state& ecs_state, const transforms::transform& camera_transform, const camera& cam);
		void blended_bind_camera(const transforms::transform& transform, const camera& cam);
		void blended_bind_light(const transforms::transform& transform, const light_directional& light);
		void blended_bind_light(const transforms::transform& transform, const light_point& light, int i);
		void blended_bind_renderable(const transforms::transform& transform, const material_pbr& material);
		void blended_unbind_renderable();

		void run_pass_cubemap(const camera& cam);
		void cubemap_bind_camera(const camera& cam);
		void cubemap_unbind();

		void run_pass_debug_colliders(ecs::state& ecs_state, const camera& cam);
		void run_pass_debug_velocity(ecs::state& ecs_state, const camera& cam, entity_id active_camera_id);
		void run_pass_animated(ecs::state &ecs_state, const transforms::transform& camera_transform, const camera &cam);
        void bind_bones(renderable_mesh_rigged& component);

	private:
		core::viewport _window_view;
		bool _is_debug = false;
		const core::frame_timer& _time;
		render_state& _render_state;
		core::glfw_context& _glfw;
		
		std::unique_ptr<render_pass> _pass_default;
		std::unique_ptr<render_pass> _pass_animated;
		std::unique_ptr<render_pass> _pass_blended;
		std::unique_ptr<render_pass> _pass_cubemap;
		std::unique_ptr<render_pass> _pass_debug;
		mesh_static _mesh_cube;
		mesh_static _mesh_sphere;
		mesh_static _mesh_arrow;
		
		glm::vec3 _debug_collider_color = glm::vec3(0, 1, 0);
		float time_between_messages = 0.25f;
		float last_message_time = 0.f;
		glm::vec3 _ambient_light_color = glm::vec3(1, 1, 1);
		float _ambient_light_intensity = 1.f;

		std::vector<glm::mat4> _bone_buffer;
	};
}

#endif
