#include "health_meter.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<health_meter>::component_bitshift = (component_shift)component_bits::health_meter;
