#include "energy_ball.hpp"
#include "component_bits.hpp"

template<> const component_shift ecs::component<components::energy_ball>::component_bitshift =
	(component_shift)components::component_bits::energy_ball;
