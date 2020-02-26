#include <rendering/skeleton_node.hpp>

void rendering::skeleton_node::compute_transform(
		glm::mat4& global_inverse,
        const glm::mat4& parent_transform,
        animation_time t,
        std::uint16_t animation_index,
        glm::mat4* buffer)
{
    auto& calculated_transform = buffer[bone_id];
	auto& animation = animations[animation_index];

	auto node_transform = animation.is_blank()
	        ? base_transform
	        : animation.eval(t);

	auto global_transform = parent_transform * node_transform;
	calculated_transform = global_transform * offset;

	for (std::uint16_t i = 0; i < child_count; ++i)
		children[i]->compute_transform(global_inverse, global_transform, t, animation_index, buffer);
}

void rendering::skeleton_node::add_child(rendering::skeleton_node* node)
{
    children[child_count++] = node;
}
