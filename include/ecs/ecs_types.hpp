//
// Created by sava on 1/15/20.
//

#ifndef WIZARDENGINE_ECS_TYPES_HPP
#define WIZARDENGINE_ECS_TYPES_HPP

#include <cstdint>
#include <bitset>

namespace ecs
{
    typedef std::uint32_t entity_id;
    typedef std::bitset<64> archetype_id;
}

#endif //WIZARDENGINE_ECS_TYPES_HPP
