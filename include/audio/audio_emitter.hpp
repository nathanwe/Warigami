//
// Created by sava on 1/27/20.
//

#ifndef WIZARDENGINE_AUDIO_EMITTER_HPP
#define WIZARDENGINE_AUDIO_EMITTER_HPP

#include <cstdint>

#include <ecs/component.hpp>

namespace audio
{
    typedef std::uint32_t playback_id;

    const std::uint32_t MaxSounds = 16;

    enum sound_state
    {
        uninitialized = 0,
        unchanged = 1,
        started,
        playback_requested,
        stop_requested
    };

    struct emitter_sound
    {
        size_t path_hash {}; // maybe don't need this
        sound_state state { uninitialized };
        playback_id playback { 0 };
        bool loop { false };
        float volume { 1.f };
    };

    struct audio_emitter : ecs::component<audio_emitter>
    {
        std::uint16_t sound_count{0};
        emitter_sound emitter_sounds[MaxSounds];

        void add_sound(size_t hash)
        {
            emitter_sounds[sound_count++] = { hash };
        }

        void set_sound_state(std::uint32_t index, sound_state state)
        {
            emitter_sounds[index].state = state;
        }
    };
}

#endif //WIZARDENGINE_AUDIO_EMITTER_HPP
