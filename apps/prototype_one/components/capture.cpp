#include "capture.hpp"

using namespace ecs;
using namespace components;

template<> 
const component_shift component<capture>::component_bitshift = 
	static_cast<component_shift>(component_bits::capture);
