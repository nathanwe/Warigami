//
// Created by sava on 1/15/20.
//

#ifndef WIZARDENGINE_MOCK_ENTITY_HPP
#define WIZARDENGINE_MOCK_ENTITY_HPP

#include <optional>

#include <ecs/entity.hpp>

namespace ecs
{
    class mock_entity
    {
    public:
        template<typename TComponent>
        TComponent &get_component()
        {
            thread_local static TComponent c;
            return c;
        }

        template<typename TComponent>
        bool has_component()
        {
            return true;
        }

        template<typename TComponent>
        std::optional<TComponent *> get_component_optional()
        {
            thread_local static TComponent c;
            return &c;
        }
    };
}

#endif //WIZARDENGINE_MOCK_ENTITY_HPP
