//
// Created by sava on 1/15/20.
//

#include <iostream>
#include <ecs/entities.hpp>

struct pos
{
    float x, y, z;
};

int main(int argc, char** argv)
{
    ecs::entities entities;
    auto& e = entities.add_entity<pos>();
    auto p = e.get_component<pos>();

    return 0;
}