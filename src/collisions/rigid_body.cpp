#include "collisions/rigid_body.hpp"

template<> const component_shift ecs::component<collisions::rigid_body>::component_bitshift = (component_shift)collisions::component_bits::rigid_body;
