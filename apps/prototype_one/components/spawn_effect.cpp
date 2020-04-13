#include "spawn_effect.hpp"
#include "component_bits.hpp"

using namespace ecs;
using namespace components;

template<> 
const component_shift component<spawn_effect>::component_bitshift =
	static_cast<component_shift>(component_bits::spawn_effect);
