//
// Created by sava on 1/23/20.
//

#include <asset/model/scene_entity.hpp>
#include <ecs/ecs_types.hpp>

json &asset::scene_entity::component(component_bitset bit)
{
    auto it = _components.find(bit)->second;
}

asset::scene_entity::scene_entity(json& entity_json) : _entity_json(entity_json)
{
    component_bitset archetype_id = 0;

    for (auto& c : entity_json["components"])
    {
        auto shift = c["component_bitshift"].get<component_shift>();
        archetype_id |= component_bitset(1) << shift;
        _components.insert(std::make_pair(shift, c));
    }
}
