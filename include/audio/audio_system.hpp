//
// Created by sava on 1/27/20.
//

#ifndef WIZARDENGINE_AUDIO_SYSTEM_HPP
#define WIZARDENGINE_AUDIO_SYSTEM_HPP

#include <ecs/system_base.hpp>
#include <fmod.hpp>



namespace audio
{
    class audio_system : public ecs::system_base
    {
    public:
        void update(ecs::state &state) override;

    private:

        FMOD::ChannelGroup *_channelGroup = nullptr;
        FMOD::System *_system = nullptr;

    };

}

#endif //WIZARDENGINE_AUDIO_SYSTEM_HPP
