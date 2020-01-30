#include <string>

#include <rendering/camera.hpp>
#include <transforms/transform.hpp>
#include <audio/audio_emitter.hpp>
#include <audio/audio_listener.hpp>
#include <ecs/register_component.hpp>
#include <ecs/archetype_pools.hpp>
#include <ecs/state.hpp>
#include <ecs/system_base.hpp>
#include <audio/audio_system.hpp>
#include <transforms/transformer.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <core/frame_timer.hpp>

const std::string SwishFile = "assets/swish.wav";


class sound_spinner : public ecs::system_base
{
public:
    sound_spinner(core::frame_timer& timer) : _timer(timer)
    {
    }

    void update(ecs::state& state)
    {
        state.each<transforms::transform, audio::audio_emitter>([this](transforms::transform& t, audio::audio_emitter e)
        {
            auto secs = _timer.delta_secs();
            glm::mat4 rotation = glm::eulerAngleZXY(t.rotation.z, t.rotation.x, secs * 0.75f);
            t.local_to_world = rotation * t.local_to_world;
        });
    }

private:
    core::frame_timer& _timer;
};

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
    auto& l = state.add_entity<transforms::transform, audio::audio_listener>();
    auto& listener = l.get_component<audio::audio_listener>();
    auto& emitter = e.get_component<audio::audio_emitter>();
    auto path_hash = strings.hash_and_store(SwishFile);

    emitter.add_sound(path_hash);
    emitter.set_sound_state(0, audio::playback_requested);
    emitter.emitter_sounds[0].loop = true;

    auto& t = e.get_component<transforms::transform>();
    t.position = {0.f, 0.f, -10.f};

    core::frame_timer timer;
    transforms::transformer transformer;
    sound_spinner spinner(timer);

    int duration = 10000000; // yolo

    while (duration-- > 0)
    {
        timer.start();

        transformer.update(state);
        spinner.update(state);
        system.update(state);

        timer.end();
    }

    return 0;
}
