//
// Created by sava on 1/23/20.
//

#include <iostream>

#include <ecs/register_component.hpp>

#include <asset/asset_manager.hpp>
#include <asset/json_cache.hpp>
#include <asset/scene.hpp>
#include <asset/scene_hydrater.hpp>

#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>

#include "sample_loader.hpp"
#include "test_actual_loaders.hpp"


const std::string ScenePath = "assets/scenes/sample_scene.json";

void test_load_scene();


int main (int argc, char** argv)
{
    ecs::register_component<transforms::transform>("transform");
    ecs::register_component<rendering::renderable_mesh_static>("renderable_mesh_static");

    test_load_scene();
    test_transform();
    test_renderable();
}

void test_load_scene()
{
    ecs::archetype_pools factory;
    ecs::state state(factory);   

    asset::asset_manager manager;
    asset::json_cache cache;
    asset::scene scene(ScenePath, cache);
    asset::scene_hydrater hydrater(state, scene);

    sample_loader dummy_loader;
    hydrater.register_loaders(&dummy_loader);

    hydrater.load();

    int counter = 0;
    
    scene.traverse([&counter](asset::scene_entity& se) {
        counter++;
    });

    if (counter != 3)
        throw std::runtime_error("scene traversal wrong");
}