#include <rendering/skeleton_node.hpp>

void rendering::skeleton_node::compute_transform(
        const glm::mat4& frame,
        animation_time t,
        std::uint16_t animation_index,
        glm::mat4* buffer)
{
	auto& animation = animations[animation_index];
	auto& calculated_transform = buffer[bone_id];
	calculated_transform = animation.eval(t) * base_transform;
		
	for (std::uint16_t i = 0; i < child_count; ++i)
		children[i]->compute_transform(calculated_transform, t, animation_index, buffer);
}

void rendering::skeleton_node::add_child(rendering::skeleton_node* node)
{
    children[child_count++] = node;
}
