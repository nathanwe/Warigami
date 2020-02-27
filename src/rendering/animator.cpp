#include <rendering/animator.hpp>

#include <iostream>

rendering::animator::animator(core::frame_timer& timer) : _timer(timer)
{
}

void rendering::animator::update(ecs::state& state)
{
	state.each<renderable_mesh_rigged>([&](renderable_mesh_rigged& component) {	
		if (component.bones.empty()) return;

		// update time
		auto anim_duration = component.find_animation_duration(component.animation_index);

		component.t += _timer.smoothed_delta();
		if (component.t > anim_duration)
			component.t = component.t - anim_duration;
	});
}
