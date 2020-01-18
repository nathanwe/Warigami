// created by sava 

#ifndef __ADD_COMPONNET_HPP_
#define __ADD_COMPONNET_HPP_

#include <functional>

#include <util/string_table.hpp>

#include <nlohmann/json.hpp>
using nlohmann::json;

namespace ecs
{
    template <typename TComponent>
    void register_component(std::function<void(const json&, ecs::entity&, util::string_table&)> load_func)
    {
        if (ecs::component_meta::bit_metas.find(TComponent::component_bitshift) !=
            ecs::component_meta::bit_metas.end())
        {
            return;
        }

        ecs::component_meta::bit_metas.insert(std::make_pair(
            TComponent::component_bitshift,
            ecs::component_meta::of<TComponent>()));

      /*  asset::component_loader::loader_functions.insert(
            std::make_pair(TComponent::component_bitshift, load_func));*/
    }
}


#endif