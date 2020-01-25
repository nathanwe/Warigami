//
// Created by sava on 10/25/19.
//

#include <asset/json_cache.hpp>
#include <fstream>

using nlohmann::json;

json asset::json_cache::load(const std::string& path)
{
    auto it = _path_to_json.find(path);

    if (it == _path_to_json.end())
    {
        std::ifstream i(path);
        json j;
        i >> j;

        _path_to_json.insert(std::make_pair(path, j));
    }

    return _path_to_json.find(path)->second;
}