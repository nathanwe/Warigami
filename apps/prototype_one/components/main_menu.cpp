#include "main_menu.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<main_menu>::component_bitshift = (component_shift)component_bits::main_menu;
