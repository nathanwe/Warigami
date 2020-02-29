//
// Created by sava on 2020-02-18.
//

#include "disco_light.h"
#include "component_bits.hpp"
#include "dice.hpp"

template<> const component_shift ecs::component<components::disco_light>::component_bitshift =
    (component_shift)components::component_bits::disco_light;
