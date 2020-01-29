//
// Created by sava on 1/23/20.
//

#ifndef WIZARDENGINE_SCENE_HPP
#define WIZARDENGINE_SCENE_HPP

#include <vector>
#include <nlohmann/json.hpp>
#include <ecs/ecs_types.hpp>

#include <asset/scene_entity.hpp>
#include <asset/resource_id.hpp>
#include <asset/json_cache.hpp>

using json = nlohmann::json;


namespace asset
{
    class scene
    {
    public:
        explicit scene(std::string file_path, json_cache& cache);

        template<typename TFunc>
        void traverse(TFunc func)
        {
            for (auto& e : _entities)
                e.traverse(func);
        }

        const std::vector<scene_entity>& entities() const { return _entities; }

    private:
        std::vector<scene_entity> _entities {};
    };

}

#endif //WIZARDENGINE_SCENE_HPP
