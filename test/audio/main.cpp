#include <string>

#include "asset/asset_manager.hpp"
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

#include <audio/loader_emitter.hpp>
#include <audio/loader_music_player.hpp>
#include <audio/music_player.hpp>


#include <asset/scene.hpp>
#include <asset/scene_hydrater.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

const std::string SwishFile = "assets/swish.wav";
const std::string SongFile = "assets/music.mp3";

class sound_spinner : public ecs::system_base
{
public:
    sound_spinner(core::frame_timer& timer) : _timer(timer)
    {
    }

    void update(ecs::state& state)
    {
        state.each<transforms::transform, audio::audio_emitter>([this](transforms::transform& t, audio::audio_emitter& e)
        {
            auto secs = _timer.delta_secs();
            t.rotation.y += secs * 0.75f;
            glm::mat4 rotation = glm::eulerAngleZXY(t.rotation.z, t.rotation.x, t.rotation.y);
            glm::mat4 translation = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -30.f));
            t.local_to_world = rotation * translation;
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
    ecs::register_component<audio::music_player>("music_player");

    asset::asset_manager assets;

    ecs::archetype_pools factory;
    ecs::state state(factory);
    util::string_table strings;
    audio::audio_system system(strings, assets);

    auto& e = state.add_entity<transforms::transform, audio::audio_emitter>();
    auto& music_entity = state.add_entity<transforms::transform, audio::music_player>();
    auto& l = state.add_entity<transforms::transform, audio::audio_listener>();
    auto& listener = l.get_component<audio::audio_listener>();
    auto& emitter = e.get_component<audio::audio_emitter>();
    auto& music_player = music_entity.get_component<audio::music_player>();
    auto path_hash = strings.hash_and_store(SwishFile);
    auto song_path_hash = strings.hash_and_store(SongFile);
    
    auto scene = assets.get_json("assets/scene.json");
    auto scene_entity = asset::scene_entity(scene["entities"][0], assets);
    auto loader_node = asset::asset_loader_node(e, scene_entity);
    audio::loader_emitter eloader(strings);
    audio::loader_music_player music_loader(strings);
    eloader.load(loader_node);
    
    emitter.emitter_sounds[0].loop = true;
    
    music_player.add_sound(song_path_hash);    
    music_player.tracks[0].loop = true;

    emitter.set_sound_state(0, audio::sound_state::playback_requested);
    //music_player.set_sound_state(0, audio::sound_state::playback_requested);


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
