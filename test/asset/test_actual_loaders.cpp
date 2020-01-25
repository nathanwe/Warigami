#include "test_actual_loaders.hpp"

#include <ecs/ecs_types.hpp>
#include <ecs/world.hpp>

#include <asset/scene.hpp>
#include <asset/scene_hydrater.hpp>

#include <transforms/transform_loader.hpp>
#include <rendering/render_loader.hpp>



const std::string ScenePath = "assets/scenes/sample_scene.json";


void test_transform()
{
    ecs::archetype_pools factory;
    ecs::state state(factory);
    
    asset::json_cache cache;
    asset::scene scene(ScenePath, cache);
    asset::scene_hydrater hydrater(state, scene);

    transforms::transform_loader transform_loader;
    hydrater.register_loaders(&transform_loader);

    hydrater.load();
}

void test_renderable()
{
    ecs::archetype_pools factory;
    ecs::state state(factory);

    asset::json_cache cache;
    asset::scene scene(ScenePath, cache);
    asset::scene_hydrater hydrater(state, scene);

    rendering::render_loader render_loader;
    hydrater.register_loaders(&render_loader);

    hydrater.load();
}