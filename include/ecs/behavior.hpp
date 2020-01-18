//
// Created by sava on 10/20/19.
//

#ifndef __BEHAVIOR_HPP_
#define __BEHAVIOR_HPP_

#include <vector>

#include <ecs/ecs_types.hpp>
#include <ecs/entity.hpp>
#include <events/event_exchange.hpp>
#include "core/behavior_context.hpp"


namespace ecs
{
    template <typename TContext>
    class behavior
    {
        const uint32_t VectorElementsReserved = 128;

    public:
        explicit behavior(events::event_exchange &events) : _events(events)
        {
            std::function<void(ecs::entity&)> on_create =
                std::bind(&behavior::_on_entity_created, this, std::placeholders::_1);

            std::function<void(ecs::entity&)> on_destroy =
                std::bind(&behavior::_on_entity_destroyed, this, std::placeholders::_1);

            _listener_id_create = events.subscribe<ecs::entity&>(events::event_type::entity_created, on_create);
            _listener_id_destroy = events.subscribe<ecs::entity&>(events::event_type::entity_destroyed, on_destroy);

            _entities.reserve(VectorElementsReserved);
            _entities_to_remove.reserve(VectorElementsReserved);
        }

        virtual ~behavior()
        {
            _events.unsubscribe(events::entity_created, _listener_id_create);
            _events.unsubscribe(events::entity_destroyed, _listener_id_destroy);
        }

        [[nodiscard]] virtual component_bitset required_components() const = 0;


        void update(TContext &ctx)
        {
            for (auto &e : _entities_to_remove)
                ctx.current_scene.remove(e);

            _entities_to_remove.clear();

            for (auto &e : _entities)
                update_single(e, ctx);
        }

    protected:
        virtual void update_single(::ecs::entity &e, TContext &ctx) = 0;

        virtual void on_entity_created(::ecs::entity &e) {}

        virtual void on_entity_destroyed(::ecs::entity &e) {}

        events::event_exchange &_events;

        void remove_entity(::ecs::entity &e)
        {
            _entities_to_remove.emplace_back(e);
        }

    private:
        std::vector<std::reference_wrapper<::ecs::entity>> _entities;
        std::vector<std::reference_wrapper<::ecs::entity>> _entities_to_remove;
        listener_id _listener_id_create;
        listener_id _listener_id_destroy;

        void _on_entity_created(::ecs::entity &e)
        {
            auto req_comps = required_components();
            auto e_comps = e.archetype_id();
            if ((req_comps & e_comps) == req_comps)
            {
                _entities.emplace_back(e);
                on_entity_created(e);
            }
        }

        void _on_entity_destroyed(::ecs::entity &e)
        {
            auto it = std::find_if(
                _entities.begin(), _entities.end(),
                [&e](::ecs::entity &x) { return x.id() == e.id(); });

            if (it != _entities.end())
            {
                on_entity_destroyed(e);
                _entities.erase(it);
            }
        }
    };
}

#endif //__BEHAVIOR_HPP_
