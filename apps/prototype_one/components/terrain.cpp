#include "terrain.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<terrain>::component_bitshift = (component_shift)component_bits::terrain;
