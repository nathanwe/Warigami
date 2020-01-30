//
// Created by sava on 1/27/20.
//

#include <iostream>

#include <ecs/system_base.hpp>
#include <audio/audio_system.hpp>
#include <fmod_errors.h>
#include <transforms/transform.hpp>
#include <rendering/camera.hpp>

bool succeededOrWarn(const std::string &message, FMOD_RESULT result);
FMOD_RESULT F_CALLBACK channelGroupCallback(FMOD_CHANNELCONTROL *channelControl,
                                            FMOD_CHANNELCONTROL_TYPE controlType,
                                            FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
                                            void *commandData1,
                                            void *commandData2);


audio::audio_system::audio_system(util::string_table &app_strings) : _app_strings(app_strings)
{
    FMOD_RESULT result;

    // Create the main system object.
    result = FMOD::System_Create(&_system);

    if (!succeededOrWarn("FMOD: Failed to create system object", result))
        throw;

    // Initialize FMOD.
    result = _system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (!succeededOrWarn("FMOD: Failed to initialise system object", result))
        throw;

    // Create the channel group.
    result = _system->createChannelGroup("inGameSoundEffects", &_channelGroup);
    if (!succeededOrWarn("FMOD: Failed to create in-game sound_wrapper effects channel group", result))
        throw;

    _system->set3DSettings(1.f, 40.f, 1.f);
}

void audio::audio_system::update(ecs::state &state)
{
    update_emitters(state);
    sync_transform_to_listener(state);
    _system->update();
}

void audio::audio_system::sync_transform_to_listener(ecs::state &state)
{
    state.each_id<transforms::transform, audio_listener>(
        [this, &state] (entity_id id, transforms::transform& t, audio_listener& l)
        {
            //auto rb_opt = listener.get_component_opt<ecs::rigid_body_component>();
            auto& entity = state.find_entity(id);
            auto cam_opt = entity.get_component_opt<rendering::camera>();

            glm::mat3 rot = cam_opt
                    ? glm::mat3(glm::inverse(cam_opt->get().view))
                    : glm::mat3(t.local_to_world);

            l.listener_position = t.position;
            l.listener_velocity = glm::vec3(0.f); //rb_opt ? rb_opt->get().velocity : glm::vec3(0.f);
            l.listener_up = rot[1];
            l.listener_forward = -rot[2];

            FMOD_VECTOR pos = {l.listener_position.x, l.listener_position.y, l.listener_position.z};
            FMOD_VECTOR vel = {l.listener_velocity.x, l.listener_velocity.y, l.listener_velocity.z};
            FMOD_VECTOR fwd = {l.listener_forward.x, l.listener_forward.y, l.listener_forward.z};
            FMOD_VECTOR up = {l.listener_up.x, l.listener_up.y, l.listener_up.z};

            _system->set3DListenerAttributes(l.listener_id, &pos, &vel, &fwd, &up);
        });
}

void audio::audio_system::update_emitters(ecs::state &state)
{
    state.each<transforms::transform, audio_emitter>([this](transforms::transform& transform, audio_emitter& e)
    {
        auto t = transform.local_to_world;
        auto t_pos = t[3];

        for (std::uint32_t i = 0; i < e.sound_count; ++i)
        {
            auto& emitter_sound = e.emitter_sounds[i];
            handle_emitter_sound(emitter_sound, t_pos);
        }
    });
}


void audio::audio_system::stop_sound(audio::emitter_sound& sound)
{
    auto& sound_channel = sound.fmod_channel;
    sound_channel->stop();
}

void audio::audio_system::play_sound_3d(audio::emitter_sound& sound)
{
    auto mode = FMOD_3D;
    if (sound.loop) mode |= FMOD_LOOP_NORMAL;
    auto fmod_sound = get_sound(sound.path_hash, mode);
    play_sound(fmod_sound, sound);
}

FMOD::Sound *audio::audio_system::get_sound(size_t hash, FMOD_MODE mode)
{
    if (_sounds.find(hash) == _sounds.end())
    {
        FMOD::Sound *sound = nullptr;
        auto& track = _app_strings[hash];
        _system->createSound(track.c_str(), mode, nullptr, &sound);
        _sounds.insert({hash, sound});
    }

    return _sounds.find(hash)->second;
}

void audio::audio_system::play_sound(FMOD::Sound *sound, audio::emitter_sound& emitter_sound)
{
    FMOD_RESULT result;

    // Play the sound_wrapper.
    FMOD::Channel *channel = nullptr;
    result = _system->playSound(sound, nullptr, false, &channel);
    if (!succeededOrWarn("FMOD: Failed to play sound_wrapper", result))
        throw;

    channel->setVolume(1.0f);

    // Assign the channel to the group.
    result = channel->setChannelGroup(_channelGroup);
    if (!succeededOrWarn("FMOD: Failed to set channel group on", result))
        throw;

    channel->setCallback(&channelGroupCallback);
    if (!succeededOrWarn("FMOD: Failed to set callback for sound_wrapper", result))
        throw;

    emitter_sound.fmod_channel = channel;
    emitter_sound.fmod_sound = sound;
}

void audio::audio_system::handle_emitter_sound(audio::emitter_sound &emitter_sound, glm::vec4& t_pos)
{
    switch (emitter_sound.state)
    {
        case sound_state::stop_requested:
        {
            stop_sound(emitter_sound);
            emitter_sound.set_null();
            break;
        }
        case sound_state::playback_requested:
        {
            play_sound_3d(emitter_sound);
        }
        default:
        {
            if (emitter_sound.is_null()) break;

            FMOD_VECTOR pos = { t_pos.x, t_pos.y, t_pos.z };
            FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
//                            rb_opt
//                                      ? FMOD_VECTOR { rb_opt->get().velocity.x, rb_opt->get().velocity.y, rb_opt->get().velocity.z }
//                                      : FMOD_VECTOR { 0.f, 0.f, 0.f };

            emitter_sound.fmod_channel->set3DAttributes(&pos, &vel);
            emitter_sound.fmod_channel->setVolume(emitter_sound.volume);
        }
    }

    emitter_sound.state = sound_state::unchanged;
}


//////////////////////
//////////////////////
bool succeededOrWarn(const std::string &message, FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cerr << message << ": " << result << " - " << FMOD_ErrorString(result) << " - " << std::endl;
        //spdlog::error("{0}: {1} {3}", message, result, FMOD_ErrorString(result));
        return false;
    }

    return true;
}

FMOD_RESULT F_CALLBACK channelGroupCallback(FMOD_CHANNELCONTROL *channelControl,
                                            FMOD_CHANNELCONTROL_TYPE controlType,
                                            FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
                                            void *commandData1,
                                            void *commandData2)
{
    // too noisy spdlog::info("FMod: channel control {0}, control type {1}");
    return FMOD_OK;
}

