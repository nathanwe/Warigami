//
// Created by sava on 2020-03-03.
//
#include "countdown.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<countdown>::component_bitshift = (component_shift)component_bits::countdown;

