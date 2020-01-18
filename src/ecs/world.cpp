//
// Created by sava on 10/15/19.
//

#include <ecs/entity.hpp>
#include <ecs/world.hpp>

ecs::world::world(archetype_pools& world) :
    _entity_pools(world)
{
    _entities.reserve(ReservedEntities);
}

ecs::entity& ecs::world::add_entity(component_bitset archetype_id)
{
    return add_entity(archetype_id, NextEntityId++);;
}


ecs::entity& ecs::world::add_entity(component_bitset archetype_id, entity_id id)
{
    auto& new_entity = _entities.emplace_back(_entity_pools.make_entity(archetype_id, id));

    for (auto& pair : _caches)
        if ((archetype_id & pair.first) == pair.first)
            pair.second.accessors.emplace_back(new_entity.id(), new_entity.accessor());

    return new_entity;
}

void ecs::world::remove_entity(entity& entity)
{
    auto archetype_id = entity.archetype_id();

    _ids.erase(entity.id());
    _entity_pools.free_entity(entity);

    for (auto& pair : _caches)
        if ((archetype_id & pair.first) == pair.first)
            pair.second.remove(entity.id());

    _entities.erase(std::remove_if(
        _entities.begin(),
        _entities.end(),
        [&entity](ecs::entity& e) { return e.id() == entity.id(); }),
        _entities.end());
}

void ecs::world::free_all()
{
    for (auto& e : _entities)
    {
        _entity_pools.free_entity(e);
    }

    _entities.clear();
}

ecs::query_cache& ecs::world::find_query_cache(component_bitset archetype)
{
    if (_caches.find(archetype) == _caches.end())
    {
        _caches.emplace(std::piecewise_construct,
            std::forward_as_tuple(archetype),
            std::forward_as_tuple(build_query_cache(archetype)));
    }

    return _caches.find(archetype)->second;
}

ecs::query_cache ecs::world::build_query_cache(component_bitset archetype)
{
    query_cache cache{ archetype };
    for (auto& e : _entities)
    {
        if ((e.archetype_id() & archetype) == archetype)
            cache.accessors.emplace_back(e.id(), e.accessor());
    }
    return cache;
}

std::atomic_uint ecs::world::NextEntityId = 100000;
