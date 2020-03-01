//
// Created by sava on 1/27/20.
//

#ifndef WIZARDENGINE_AUDIO_SYSTEM_HPP
#define WIZARDENGINE_AUDIO_SYSTEM_HPP

#include "asset/asset_manager.hpp"
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
        explicit audio_system(util::string_table &app_strings, asset::asset_manager& assets);

        void update(ecs::state &state) override;

    private:
        asset::asset_manager& _assets;
        FMOD::ChannelGroup *_channelGroup { nullptr };
        FMOD::System *_system = { nullptr };
        std::unordered_map<size_t, FMOD::Sound *> _sounds {};
        util::string_table &_app_strings;

        void sync_transform_to_listener(ecs::state &state);
        void update_emitters(ecs::state &state);
        void stop_sound(audio::emitter_sound& sound);
        void play_sound_3d(audio::emitter_sound& sound);
        void play_sound_non3d(audio::emitter_sound& sound);
        void play_sound(FMOD::Sound *sound, audio::emitter_sound& emitter);
        void handle_emitter_sound(emitter_sound& sound, glm::vec3& t_pos, glm::vec3& velocity);
        void handle_music_sound(emitter_sound& sound);
        void check_sound_stopped(emitter_sound& sound);
        FMOD::Sound *get_sound(size_t hash, FMOD_MODE mode);
    };
}


#endif //WIZARDENGINE_AUDIO_SYSTEM_HPP
