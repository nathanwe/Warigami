//
// Created by sava on 10/8/19.
//

#ifndef __ENTITY_WORLD_H_
#define __ENTITY_WORLD_H_

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <vector>
#include <functional>
#include <atomic>


#include <ecs/entity.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/archetype_pools.hpp>
#include <ecs/archetype_id.hpp>
#include <ecs/query_cache.hpp>


namespace ecs
{
    /**
     * This is the primary class for storing and querying entities.
     */
    class world
    {
        const static std::uint32_t ReservedEntities = 4096;
        static std::atomic_uint NextEntityId;

    public:
        world(archetype_pools& world);

        template <typename... TComponents>
        entity& add_entity(entity_id id)
        {
            auto archetype = ecs::archetype_id<TComponents...>();
            return add_entity(archetype, id);
        }

        template <typename... TComponents>
        entity& add_entity()
        {
            constexpr auto archetype = ecs::archetype_id<TComponents...>();
            return add_entity(archetype);
        }

        entity& add_entity(component_bitset archetype_id, entity_id id);
        entity& add_entity(component_bitset archetype_id);

        void remove_entity(entity& entity);

        query_cache build_query_cache(component_bitset archetype);


        template <typename... TComponents, typename TFunc>
        void each(TFunc callback)
        {
            constexpr auto arch_id = ecs::archetype_id<TComponents...>();
            auto& cache = find_query_cache(arch_id);

            for (auto a : cache.accessors)
            {
                callback((*(a.accessor.template get_component<TComponents>()))...);
            }
        }

        void free_all();

    private:
        archetype_pools &_entity_pools;        
        std::unordered_set<entity_id> _ids;
        std::unordered_map<component_bitset, query_cache> _caches;
        std::vector<entity> _entities;

        query_cache& find_query_cache(component_bitset archetype);
    };
}

#endif //__ENTITY_WORLD_H_
