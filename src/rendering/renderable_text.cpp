#include <rendering/renderable_text.hpp>
#include <rendering/component_bits.hpp>
#include <rendering/renderable_text.hpp>

using namespace ecs;
using namespace rendering;

template<> const component_shift component<renderable_text>::component_bitshift = (component_shift)component_bits::renderable_text;
