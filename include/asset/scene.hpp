//
// Created by sava on 1/23/20.
//

#ifndef WIZARDENGINE_SCENE_HPP
#define WIZARDENGINE_SCENE_HPP

#include <vector>
#include <nlohmann/json.hpp>
#include <ecs/ecs_types.hpp>

#include "asset/asset_manager.hpp"
#include <asset/scene_entity.hpp>
#include <asset/resource_id.hpp>

using json = nlohmann::json;


namespace asset
{
    class scene
    {
    public:
        explicit scene(std::string file_path, asset_manager& assets);

        template<typename TFunc>
        void traverse(TFunc func)
        {
            for (auto& e : _entities)
                e.traverse(func);
        }

        scene_entity& add_from_prototype(const std::string& path);

        [[nodiscard]] const std::vector<scene_entity>& entities() const { return _entities; }

    private:
        asset_manager& _assets;

        std::vector<scene_entity> _entities {};
    };

}

#endif //WIZARDENGINE_SCENE_HPP
