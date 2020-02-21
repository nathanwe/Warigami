#include <rendering/animator.hpp>

rendering::animator::animator(core::frame_timer& timer) : _timer(timer)
{
}

void rendering::animator::update(ecs::state& state)
{
	state.each<renderable_mesh_rigged>([&](renderable_mesh_rigged& component) {	
		if (component.bones.empty()) return;

		// update time
		auto anim_duration = component.bones[0].animations[component.animation_index].duration;
		component.t += _timer.smoothed_delta();
		if (component.t > anim_duration)
			component.t = component.t - anim_duration;

		// update matrices
		component.root->compute_transform(glm::mat4(1.f), component.t, component.animation_index);
	});
}
