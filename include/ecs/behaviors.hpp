//
// Created by sava on 1/15/20.
//

#ifndef WIZARDENGINE_BEHAVIORS_HPP
#define WIZARDENGINE_BEHAVIORS_HPP

#include <vector>
#include <cstdint>

#include <ecs/behavior_base.hpp>

namespace ecs
{
    template<typename TContext>
    class behaviors
    {
        const std::uint32_t NumReservedBehaviorPointers = 16;

    public:
        behaviors()
        {
            _behaviors.reserve(NumReservedBehaviorPointers);
        }

        void update(TContext &context)
        {
            for (auto* behavior : _behaviors) {
                behavior->update(context);
            }
        }

        void register_behavior(behavior_base<TContext> * behavior)
        {
            _behaviors.push_back(behavior);
        }


    private:
        std::vector<behavior_base<TContext> *> _behaviors;
    };

}

#endif //WIZARDENGINE_BEHAVIORS_HPP
