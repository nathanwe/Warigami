#include "terrain_graphic.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<terrain_graphic>::component_bitshift = (component_shift)component_bits::terrain_graphic;
