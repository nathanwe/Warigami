#include <ecs/systems.hpp>
#include <ecs/state.hpp>


void ecs::systems::update(ecs::state& state)
{
	for (auto* sys : _systems)
		sys->update(state);
}
