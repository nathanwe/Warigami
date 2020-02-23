//
// Created by sava on 2020-02-18.
//

#include <iostream>

#include <rendering/renderable_mesh_rigged.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/component.hpp>
#include <rendering/component_bits.hpp>


template<> const component_shift ecs::component<rendering::renderable_mesh_rigged>::component_bitshift =
        (component_shift) rendering::component_bits::renderable_mesh_rigged;


rendering::animation_time rendering::renderable_mesh_rigged::find_animation_duration(std::uint32_t index) const
{
    for (auto &b : bones)
        if (b.animations[index].duration != animation_time::zero())
            return b.animations[index].duration;

    return rendering::animation_time::zero();
}

void rendering::renderable_mesh_rigged::compute_transform(glm::mat4 *buffer)
{
    root->compute_transform(global_inverse, glm::mat4(1.f), t, animation_index, buffer);
}