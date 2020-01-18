#ifndef __QUERY_CACHE_HPP_
#define __QUERY_CACHE_HPP_

#include <vector>

#include <ecs/ecs_types.hpp>
#include <ecs/chunk_component_accessor.hpp>
#include <ecs/entity.hpp>

namespace ecs
{
	struct entity_accessor
	{
		entity_accessor(entity_id id, chunk_component_accessor a) : entity(id), accessor(a) {}

		entity_id entity;
		chunk_component_accessor accessor;
	};

	struct query_cache
	{	
		component_bitset archetype;
		std::vector<entity_accessor> accessors{};

		void remove(entity_id id)
		{
			accessors.erase(std::remove_if(
					accessors.begin(), 
					accessors.end(), 
					[id](entity_accessor& a) {
						return a.entity == id;
					}), 
				accessors.end());
		}
	};
}

#endif // !__QUERY_CACHE_HPP_
