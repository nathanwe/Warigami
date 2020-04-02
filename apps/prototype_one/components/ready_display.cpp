#include "ready_display.hpp"
#include "component_bits.hpp"

template<> const component_shift ecs::component<components::ready_display>::component_bitshift =
	(component_shift)components::component_bits::ready_display;
