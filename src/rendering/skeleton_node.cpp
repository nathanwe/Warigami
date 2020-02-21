#include <rendering/skeleton_node.hpp>

void rendering::skeleton_node::compute_transform(const glm::mat4& frame, animation_time t, std::uint16_t animation_index)
{
	auto& animation = animations[animation_index];
	calculated_transform = animation.eval(t) * base_transform;
		
	for (std::uint16_t i = 0; i < child_count; ++i)
		children[i]->compute_transform(calculated_transform, t, animation_index);
}

void rendering::skeleton_node::add_child(rendering::skeleton_node* node)
{
    children[child_count++] = node;
}