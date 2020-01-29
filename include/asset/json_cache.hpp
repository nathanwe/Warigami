//
// Created by sava on 10/25/19.
//

#ifndef __PROTOTYPE_LOADER_HPP_
#define __PROTOTYPE_LOADER_HPP_

#include <unordered_map>

#include <nlohmann/json.hpp>
using nlohmann::json;

namespace asset
{
    class json_cache
    {
    public:
        json load(const std::string& path);
        void clear() { _path_to_json.clear(); }

    private:
        std::unordered_map<std::string, json> _path_to_json;
    };

}


#endif //__PROTOTYPE_LOADER_HPP_