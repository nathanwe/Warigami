#ifndef WIZARDPEOPLE_COLLISIONS_COMPONENT_BITS_HPP
#define WIZARDPEOPLE_COLLISIONS_COMPONENT_BITS_HPP

#include <ecs/ecs_types.hpp>

namespace collisions
{
    enum class component_bits : component_shift
    {
        aabb_collider = 25,
        sphere_collider = 26,        
        rigid_body = 27
    };
}

#endif