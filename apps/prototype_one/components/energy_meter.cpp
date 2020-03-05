#include "energy_meter.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<energy_meter>::component_bitshift = (component_shift)component_bits::energy_meter;
