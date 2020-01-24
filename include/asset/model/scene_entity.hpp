//
// Created by sava on 1/23/20.
//

#ifndef WIZARDENGINE_SCENE_ENTITY_HPP
#define WIZARDENGINE_SCENE_ENTITY_HPP

#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <ecs/ecs_types.hpp>

using json = nlohmann::json;

namespace asset
{

    class scene_entity
    {
    public:
        scene_entity(json &entity_json);

        json &component(component_bitset bit);

        [[nodiscard]] component_bitset archetype_id() const
        { return _archetype_id; }

    private:
        std::map<component_bitset, json> _components;
        json _entity_json;
        std::vector<scene_entity> _children;
        component_bitset _archetype_id;
    };

}

#endif //WIZARDENGINE_SCENE_ENTITY_HPP
