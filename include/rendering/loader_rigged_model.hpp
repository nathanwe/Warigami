//
// Created by sava on 2020-02-18.
//

#ifndef WIZARDENGINE_LOADER_RIGGED_MODEL_HPP
#define WIZARDENGINE_LOADER_RIGGED_MODEL_HPP

#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>

namespace rendering
{
    class loader_rigged_model : public asset::component_loader
    {
        void load(asset::asset_loader_node& asset_loader_node) override;

        component_bitset components_to_load() override;
    };
}


#endif //WIZARDENGINE_LOADER_RIGGED_MODEL_HPP
