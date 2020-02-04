//
// Created by sava on 1/23/20.
//

#include <fstream>

#include <asset/scene.hpp>
#include <asset/resource_id.hpp>

asset::scene::scene(std::string file_path, json_cache& cache)
{
	// read a JSON file
	auto j = cache.load(file_path);

	std::vector<json> descendant_children;
	std::vector<scene_entity*> inserted_children;
    
    _entities.reserve(j["entities"].size());

	for (auto entity : j["entities"])
	{
		if (entity.find("parent_id") == entity.end())
		{
			_entities.emplace_back(entity, cache);
			inserted_children.push_back(&_entities.back());
		}
		else
		{
			descendant_children.push_back(entity);
		}
	}

    while (!descendant_children.empty())
    {
        auto no_change = true;

        for (auto* potential_parent : inserted_children)
        {
            auto jsonstr = potential_parent->j().dump();

            auto parent_id = potential_parent->id();

            for (auto& descendant : descendant_children)
            {
                auto descendant_parent_id = descendant["parent_id"].get<entity_id>();
                if (descendant_parent_id == parent_id)
                {
                    auto& inserted = potential_parent->add_child(descendant, cache);
                    inserted_children.push_back(&inserted);
                    no_change = false;
                }
            }

            auto removal = std::remove_if(descendant_children.begin(), descendant_children.end(),
                [parent_id](json& j) { return j["parent_id"].get<std::uint32_t>() == parent_id; });
            descendant_children.erase(removal, descendant_children.end());
        }

        if (no_change) break;
    }
}
