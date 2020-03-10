#include "tug_of_war_meter.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<tug_of_war_meter>::component_bitshift = (component_shift)component_bits::tug_of_war_meter;
