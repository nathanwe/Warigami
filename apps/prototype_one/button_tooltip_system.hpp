#pragma once

#include <ecs/state.hpp>
#include <ecs/system_base.hpp>
#include <ecs/entity.hpp>

#include "components/player.hpp"

class button_tooltip_system : public ecs::system_base
{
public:

	void initialize(ecs::state& state);
	void update(ecs::state& state);

private:
	ecs::entity* _players[2]{nullptr, nullptr};

};