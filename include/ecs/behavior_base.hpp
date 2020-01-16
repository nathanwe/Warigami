//
// Created by sava on 1/15/20.
//

#ifndef WIZARDENGINE_BEHAVIOR_HPP
#define WIZARDENGINE_BEHAVIOR_HPP

namespace ecs
{

    template <typename TContext>
    class behavior_base
    {
    public:
        virtual void update(TContext& context) = 0;
    };

}

#endif //WIZARDENGINE_BEHAVIOR_HPP
