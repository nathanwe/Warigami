#include <asset/scene_hydrater.hpp>

asset::scene_hydrater::scene_hydrater(ecs::state& ecs_state, asset::scene& scene) 
	: _ecs_state(ecs_state)
	, _scene(scene)
{
	for (auto& scene_entity : scene.entities())
	{
		auto& ecs_entity = scene_entity.has_id()
			? ecs_state.add_entity(scene_entity.archetype_id(), scene_entity.id())
			: ecs_state.add_entity(scene_entity.archetype_id());

		auto& graph_entity = _entity_refs.emplace_back(ecs_entity, scene_entity);
		hydrate_recurse(scene_entity, graph_entity, ecs_state);
	}
}

void asset::scene_hydrater::hydrate_recurse(const scene_entity& entity, asset_loader_node& graph_entity, ecs::state& ecs_state)
{
	for (auto& c : entity.children())
	{
		auto& ecs_entity = entity.has_id()
			? ecs_state.add_entity(entity.archetype_id(), entity.id() )
			: ecs_state.add_entity(entity.archetype_id());

		auto& graph_child = graph_entity.children.emplace_back(ecs_entity, c);
		hydrate_recurse(c, graph_child, ecs_state);
	}
}

void asset::scene_hydrater::load_recurse(asset_loader_node& entity)
{
	for (auto* loader : _component_loaders)
	{
		auto arch = loader->components_to_load();
		auto& entity_r = entity.entity_resource.entity_data;
		auto entity_arch = entity.entity_resource.entity.archetype_id();
		
		if ((arch & entity_arch) == arch)
			loader->load(entity);

		for (auto& child : entity.children)
			load_recurse(child);
	}
}