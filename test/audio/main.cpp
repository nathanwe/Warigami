#include <rendering/camera.hpp>
#include <transforms/transform.hpp>
#include <audio/audio_emitter.hpp>
#include <audio/audio_listener.hpp>
#include <ecs/register_component.hpp>
#include <ecs/archetype_pools.hpp>
#include <ecs/state.hpp>
#include <audio/audio_system.hpp>

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



    return 0;
}