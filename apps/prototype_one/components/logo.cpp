#include "logo.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<logo>::component_bitshift = (component_shift)component_bits::logo;
