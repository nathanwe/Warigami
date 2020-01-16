//
// Created by sava on 1/15/20.
//

#ifndef WIZARDENGINE_MOCK_ENTITIES_HPP
#define WIZARDENGINE_MOCK_ENTITIES_HPP

#include <ecs/entity.hpp>
#include <vector>

namespace ecs
{
    class mock_entities
    {
    public:
        template<typename TFunc, typename... TComponent>
        void each(TFunc f)
        {
            for (auto& e : _entities) f(e);
        }

        template<typename TFunc, typename... TComponent>
        void parallel_each(TFunc f)
        {
            for (auto& e : _entities) f(e);
        }

        template<typename... TComponent>
        entity& add_entity()
        {
            _entities.push_back(entity());
            return _entities.back();
        }

        template<typename... TComponent>
        entity& add_entity(entity_id id)
        {
            _entities.push_back(entity());
            return _entities.back();
        }

        entity& add_entity(archetype_id archetype)
        {
            _entities.push_back(entity());
            return _entities.back();
        }

        entity& add_entity(archetype_id archetype, entity_id id)
        {
            _entities.push_back(entity());
            return _entities.back();
        }

    private:
        std::vector<entity> _entities;
    };
}

#endif //WIZARDENGINE_MOCK_ENTITIES_HPP
