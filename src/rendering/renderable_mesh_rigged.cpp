//
// Created by sava on 2020-02-18.
//

#include <rendering/renderable_mesh_rigged.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/component.hpp>
#include <rendering/component_bits.hpp>


template<> const component_shift ecs::component<rendering::renderable_mesh_rigged>::component_bitshift =
    (component_shift)rendering::component_bits::renderable_mesh_rigged;
