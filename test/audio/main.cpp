#include <string>

#include <rendering/camera.hpp>
#include <transforms/transform.hpp>
#include <audio/audio_emitter.hpp>
#include <audio/audio_listener.hpp>
#include <ecs/register_component.hpp>
#include <ecs/archetype_pools.hpp>
#include <ecs/state.hpp>
#include <audio/audio_system.hpp>

const std::string SwishFile = "assets/swish.wav";

int main()
{
    ecs::register_component<transforms::transform>("transform");
    ecs::register_component<rendering::camera>("camera");
    ecs::register_component<audio::audio_listener>("audio_listener");
    ecs::register_component<audio::audio_emitter>("audio_emitter");

    ecs::archetype_pools factory;
    ecs::state state(factory);
    util::string_table strings;
    audio::audio_system system(strings);

    auto& e = state.add_entity<transforms::transform, audio::audio_emitter, audio::audio_listener>();
    auto& listener = e.get_component<audio::audio_listener>();
    auto& emitter = e.get_component<audio::audio_emitter>();
    auto path_hash = strings.hash_and_store(SwishFile);
    emitter.add_sound(path_hash);
    emitter.set_sound_state(0, audio::playback_requested);

    int duration = 1000000000;

    while (duration-- > 0)
    {
        system.update(state);
    }

    return 0;
}