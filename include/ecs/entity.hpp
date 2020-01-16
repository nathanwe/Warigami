//
// Created by sava on 1/15/20.
//

#ifndef WIZARDENGINE_ENTITY_BASE_HPP
#define WIZARDENGINE_ENTITY_BASE_HPP

#include <optional>

#include <ecs/ecs_types.hpp>
#include <ecs/policies/mock_entity.hpp>

namespace ecs
{
    class entity : public mock_entity
    {
    public:
        entity_id id() { return _id; }

        void set_active(bool val) { _is_active = val; }

        bool is_active() const { return _is_active; }


    private:

        entity_id _id;
        bool _is_active {true};
    };
}

#endif //WIZARDENGINE_ENTITY_BASE_HPP
