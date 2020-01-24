//
// Created by sava on 1/23/20.
//

#ifndef WIZARDENGINE_SCENE_HPP
#define WIZARDENGINE_SCENE_HPP

#include <unordered_map>
#include <nlohmann/json.hpp>
#include <ecs/ecs_types.hpp>

#include <asset/model/scene_entity.hpp>
#include <asset/resource_id.hpp>

using json = nlohmann::json;


namespace asset
{

    class scene
    {
    public:
        explicit scene(std::string file_path);

    private:
        std::unordered_map<resource_id, scene_entity> _entities {};
    };

}

#endif //WIZARDENGINE_SCENE_HPP
