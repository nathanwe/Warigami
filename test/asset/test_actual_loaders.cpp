#include "test_actual_loaders.hpp"

#include <ecs/ecs_types.hpp>
#include <ecs/world.hpp>

#include <asset/asset_manager.hpp>
#include <asset/scene.hpp>
#include <asset/scene_hydrater.hpp>

#include <transforms/transform_loader.hpp>
#include <rendering/render_loader.hpp>

const std::string ScenePath = "assets/scenes/sample_scene.json";


void test_transform()
{
    ecs::archetype_pools factory;
    ecs::state state(factory);
    
    asset::asset_manager assets;
    asset::scene scene(ScenePath, assets);
    asset::scene_hydrater hydrater(state);

    transforms::transform_loader transform_loader;
    hydrater.register_loaders(&transform_loader);

    hydrater.load();
}

void test_renderable()
{
    ecs::archetype_pools factory;
    ecs::state state(factory);

    asset::asset_manager assets;
    asset::scene scene(ScenePath, assets);
    asset::scene_hydrater hydrater(state);

    rendering::asset_cache render_asset_cache(assets);
    rendering::render_loader render_loader(render_asset_cache);
    hydrater.register_loaders(&render_loader);

    hydrater.load();
}