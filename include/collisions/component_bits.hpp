#ifndef WIZARDPEOPLE_COLLISIONS_COMPONENT_BITS_HPP
#define WIZARDPEOPLE_COLLISIONS_COMPONENT_BITS_HPP

#include <ecs/ecs_types.hpp>

namespace collisions
{
    enum class component_bits : component_shift
    {
        collider = 25,
        rigidbody = 26
    };
}

#endif