//
// Created by sava on 2020-03-06.
//

#ifndef WIZARDENGINE_LOADER_RENDERABLE_TEXT_HPP
#define WIZARDENGINE_LOADER_RENDERABLE_TEXT_HPP

#include <util/string_table.hpp>
#include "asset/component_loader.hpp"
#include "rendering/asset_cache.hpp"

namespace rendering
{
    class loader_renderable_text : public asset::component_loader
    {
    public:
        explicit loader_renderable_text(util::string_table& strings);

        component_bitset components_to_load() override;
        void load(asset::asset_loader_node& asset_loader_node) override;

    private:
        util::string_table& _strings;
    };
}

#endif //WIZARDENGINE_LOADER_RENDERABLE_TEXT_HPP
