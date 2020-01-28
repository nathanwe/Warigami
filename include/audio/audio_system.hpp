//
// Created by sava on 1/27/20.
//

#ifndef WIZARDENGINE_AUDIO_SYSTEM_HPP
#define WIZARDENGINE_AUDIO_SYSTEM_HPP

#include <audio/audio_listener.hpp>
#include <audio/audio_emitter.hpp>
#include <ecs/system_base.hpp>
#include <fmod.hpp>

#include <util/string_table.hpp>

#include <glm/vec4.hpp>

namespace audio
{
    class audio_system : public ecs::system_base
    {
    public:
        explicit audio_system(util::string_table &app_strings);

        void update(ecs::state &state) override;

    private:
        FMOD::ChannelGroup *_channelGroup { nullptr };
        FMOD::System *_system = { nullptr };
        std::unordered_map<playback_id, std::pair<FMOD::Sound *, FMOD::Channel *>> _playbacks {};
        std::unordered_map<size_t, FMOD::Sound *> _sounds {};
        std::unordered_map<size_t, playback_id> _sound_to_playback {};
        std::atomic_uint _next_playback_id { 1 };

        util::string_table &_app_strings;

        void sync_transform_to_listener(ecs::state &state);
        void update_emitters(ecs::state &state);
        void stop_sound(playback_id playback);
        void stop_all();
        audio::playback_id play_sound_3d(size_t path_hash, bool loop);
        FMOD::Sound *get_sound(size_t hash, FMOD_MODE mode);
        audio::playback_id play_sound(FMOD::Sound *sound, size_t path_hash);
        void handle_emitter_sound(emitter_sound& sound, glm::vec4& t_pos);

    };
}


#endif //WIZARDENGINE_AUDIO_SYSTEM_HPP
