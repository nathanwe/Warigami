//
// Created by sava on 1/23/20.
//

#include <unordered_set>

#include <asset/scene_entity.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/component_meta.hpp>

asset::scene_entity::scene_entity(json& entity_json, json_cache& cache)
    : _id(asset::next_resource_id())
    , _entity_json(entity_json)
    , _archetype_id(0)
{
    auto prototype = inflate_prototype(entity_json, cache);    
       
    for (auto& c : prototype["root"]["components"])
    {
        auto type = c["type"].get<std::string>();
        auto shift = ecs::component_meta::type_to_bit[type];
        _archetype_id |= component_bitset(1) << shift;
        _components.insert(std::make_pair(shift, c));
    }

    for (auto& c : prototype["children"])
    {
        _children.emplace_back(c, cache);
    }
}

json& asset::scene_entity::component(component_bitset bit)
{
    return _components.find(bit)->second;
}

json asset::scene_entity::inflate_prototype(json& entity_json, json_cache& cache)
{
    auto entity_components = entity_json["components"];
    auto prototype_path_it = entity_json.find("prototype");

    if (prototype_path_it == entity_json.end())
        return { {"root", entity_json}, {"children", json::array()} };

    auto prototype_path = prototype_path_it.value().get<std::string>();
    json prototype_json = cache.load(prototype_path);
    merge_component_props(prototype_json["root"], entity_json);
    return prototype_json;;
}

void asset::scene_entity::merge_component_props(json& target, json& source)
{
    std::unordered_set<std::uint8_t> target_component_shifts;
    std::unordered_map<std::uint8_t, json> shift_to_source_component;

    auto& source_components = source["components"];
    auto& target_components = target["components"];

    // keep track of scene entity components.
    for (auto& c : source_components)
    {
        auto type = c["type"].get<std::string>();
        auto shift = ecs::component_meta::type_to_bit[type];
        shift_to_source_component[shift] = c;
    }

    // override prototype component props with scene entity component props
    for (auto& target_component : target_components)
    {
        auto type = target_component["type"].get<std::string>();
        auto shift = ecs::component_meta::type_to_bit[type];
        target_component_shifts.insert(shift);

        auto it = shift_to_source_component.find(shift);
        if (it == shift_to_source_component.end()) continue;

        auto& entity_component = it->second;
        for (auto& el : entity_component.items())
        {
            target_component[el.key()] = el.value();
        }
    }

    // insert components that are in scene, but not prototype
    for (auto& c : source_components)
    {
        auto shift = c["component_bitshift"].get<std::uint8_t>();
        if (target_component_shifts.find(shift) == target_component_shifts.end())
        {
            target_components.push_back(c);
        }
    }
}