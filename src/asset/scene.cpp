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

	for (auto entity : j["entities"])
		_entities.push_back(scene_entity(entity, cache));
}
