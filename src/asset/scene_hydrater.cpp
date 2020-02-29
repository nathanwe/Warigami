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
		hydrate_recurse(graph_entity);
	}
}

void asset::scene_hydrater::hydrate_recurse(asset_loader_node& graph_entity)
{
	for (auto& c : graph_entity.entity_resource.entity_data.children())
	{
		auto& ecs_entity = c.has_id()
			? _ecs_state.add_entity(c.archetype_id(), c.id() )
			: _ecs_state.add_entity(c.archetype_id());

		auto& graph_child = graph_entity.children.emplace_back(ecs_entity, c);
		hydrate_recurse(graph_child);
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

ecs::entity& asset::scene_hydrater::add_from_prototype(const std::string& path)
{
    auto& scene_entity = _scene.add_from_prototype(path);
    auto& ecs_entity = _ecs_state.add_entity(scene_entity.archetype_id());
    auto& graph_entity = _entity_refs.emplace_back(ecs_entity, scene_entity);
    hydrate_recurse(graph_entity);
    load_recurse(graph_entity);
    return ecs_entity;
}
