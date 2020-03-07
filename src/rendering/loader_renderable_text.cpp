//
// Created by sava on 2020-03-06.
//

#include <rendering/loader_renderable_text.hpp>
#include <rendering/renderable_text.hpp>

rendering::loader_renderable_text::loader_renderable_text(util::string_table &strings)
    : _strings(strings)
{
}

component_bitset rendering::loader_renderable_text::components_to_load()
{
    return renderable_text::archetype_bit;
}

void rendering::loader_renderable_text::load(asset::asset_loader_node &asset_loader_node)
{
    auto entity = asset_loader_node.entity_resource.entity;
    auto entity_data = asset_loader_node.entity_resource.entity_data;
    auto& json = entity_data->component_data(renderable_text::component_bitshift);
    auto& renderable = entity->get_component<renderable_text>();

    renderable.scale = json.value("scale", 1.f);
    renderable.string_hash = _strings.hash_and_store(json.value("text", ""));

    if (json.find("position") != json.end())
        renderable.position = {
        json["position"][0].get<int>(),
        json["position"][1].get<int>() };

    if (json.find("color") != json.end())
        renderable.color = {
                json["color"][0].get<float>(),
                json["color"][1].get<float>(),
                json["color"][2].get<float>()};
}
