//
// Created by sava on 1/23/20.
//

#include <fstream>

#include <asset/scene.hpp>
#include <asset/resource_id.hpp>
#include <core/logger.hpp>


asset::scene::scene(std::string file_path, asset_manager& assets) : _assets(assets)
{
    core::logger::log("loading scene: " + file_path);

	// read a JSON file
	auto j = assets.get_json(file_path);    

	std::vector<json> descendant_children;
	std::vector<scene_entity*> inserted_children;

    inserted_children.reserve(j["entities"].size()*512);
    descendant_children.reserve(j["entities"].size()*512);
    
    std::ostringstream osstop;
    osstop << "reserving: " << j["entities"].size() * 50;
    core::logger::log(osstop.str());

    _entities.reserve(j["entities"].size());

    int parents_count = 0;
    int descendant_count = 0;

	for (auto entity : j["entities"])
	{
		if (entity.find("parent_id") == entity.end())
		{
            std::ostringstream oss;
            oss << "descendants: " << parents_count++;
            core::logger::log(oss.str());

			_entities.emplace_back(entity, assets);
			inserted_children.push_back(&_entities.back());
		}
		else
		{
            std::ostringstream oss;
            oss << "descendants: " << descendant_count++;
            core::logger::log(oss.str());
			descendant_children.push_back(entity);
		}
	}

    while (!descendant_children.empty())
    {
        auto no_change = true;

        for (auto* potential_parent : inserted_children)
        {
            core::logger::log(potential_parent->j().dump());

            if (!potential_parent->has_id()) continue;

            auto parent_id = potential_parent->id();

            for (auto& descendant : descendant_children)
            {
                auto descendant_parent_id = descendant["parent_id"].get<entity_id>();
                if (descendant_parent_id == parent_id)
                {
                    auto& inserted = potential_parent->add_child(descendant, assets);
                    inserted_children.push_back(&inserted);

                    std::ostringstream oss2;
                    oss2 << "inserted count: " << inserted_children.size();
                    core::logger::log(oss2.str());

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

asset::scene_entity& asset::scene::add_from_prototype(const std::string& file_path)
{
    auto j = _assets.get_json(file_path);
    return _entities.emplace_back(j, _assets);
}

