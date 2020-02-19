//
// Created by sava on 2020-02-18.
//

#include <rendering/loader_rigged_model.hpp>
#include <rendering/renderable_mesh_rigged.hpp>


void rendering::loader_rigged_model::load(asset::asset_loader_node& asset_loader_node)
{
    auto& entity = asset_loader_node.entity_resource.entity;
    auto& entity_data = asset_loader_node.entity_resource.entity_data;
    auto& json = entity_data.component_data(rendering::renderable_mesh_rigged::component_bitshift);
    auto& mesh = entity.get_component<rendering::renderable_mesh_rigged>();


}

component_bitset rendering::loader_rigged_model::components_to_load()
{
    return rendering::renderable_mesh_rigged::archetype_bit;
}
