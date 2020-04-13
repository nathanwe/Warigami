#include "deck_cursor.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> const component_shift component<deck_cursor>::component_bitshift = 
	(component_shift)component_bits::deck_cursor;

