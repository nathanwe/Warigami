#include <cstdio>
#include <cstring>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ecs/ecs_types.hpp>
#include <ecs/world.hpp>

#include "components.h"

void test_basic();
void test_each();


int test_ecs_world_all()
{
    ecs::register_component<transform>(dummy_load);
    ecs::register_component<renderdata>(dummy_load);

    test_basic();
    test_each();
   
    printf("Done with ecs world tests.\n");
    return 0;
}

void test_basic()
{
    unsigned int  count = 100;

    while (count-- > 0)
    {
        ecs::archetype_pools factory;
        ecs::world world(factory);

        auto& entityA = world.add_entity<transform, renderdata>();
        auto& entityB = world.add_entity<transform>();

        auto& transformA = entityA.get_component<transform>();
        auto& transformB = entityB.get_component<transform>();
        auto& renderA = entityA.get_component<renderdata>();
    }
}

void test_each()
{    
    unsigned int  count = 100;

    while (count-- > 0)
    {
        ecs::archetype_pools factory;
        ecs::world world(factory);

        auto& entityA = world.add_entity<transform, renderdata>();
        auto& entityB = world.add_entity<transform, renderdata>();
        auto& entityC = world.add_entity<transform>();

        auto& transformA = entityA.get_component<transform>();
        auto& transformB = entityB.get_component<transform>();
        auto& renderA = entityA.get_component<renderdata>();

        int iter_count = 0;

        world.each<transform, renderdata>([&iter_count](transform& t, renderdata& r) {
            iter_count++;
            });

        if (iter_count != 2) throw std::runtime_error("iteration is wrong");

        iter_count = 0;

        world.each<transform>([&iter_count](transform& t) {
            iter_count++;
            t.pitch = 123;
            });

        if (iter_count != 3) throw std::runtime_error("iteration is wrong");

        world.each<transform>([&iter_count](transform& t) {
            if (t.pitch != 123)
                throw std::runtime_error("transform pitch values should all be 123");
            });
    }
}