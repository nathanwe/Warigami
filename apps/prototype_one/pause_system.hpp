#ifndef WIZARD_GAME_PAUSE_SYSTEM_HPP
#define WIZARD_GAME_PAUSE_SYSTEM_HPP

#include <asset/scene_hydrater.hpp>
#include <audio/audio_system.hpp>
#include <core/frame_timer.hpp>
#include <core/game_input_manager.hpp>
#include <core/glfw_context.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <rendering/camera.hpp>
#include <transforms/transform.hpp>
#include <asset/scene_change_event.hpp>
#include <event/event_manager.hpp>

#include "components/pause.hpp"


enum pause_options {
	RESUME,
	REPLAY,
	MAIN_MENU,
	QUIT,
	HOW_TO,
	OPTIONS,
	CREDITS,
	NUM_PAUSE_OPTIONS
};


class pause_system : public ecs::system_base
{
public:
	pause_system(
		core::game_input_manager& input,
		core::frame_timer& timer,
		core::glfw_context& glfw,
		asset::scene_hydrater& hydrater,
		event::EventManager& events,
		audio::audio_system& audioo,
		core::startup_config& config)
		: m_r_input(input),
		m_r_timer(timer),
		m_r_glfw(glfw),
		m_r_hydrater(hydrater),
		m_r_events(events),
		_current_selection(0),
		_seeing_message(false),
		current_page(0),
		how_to_page(0),
		credits_page(0),
		codex_page(0),
		_current_options_selection(0),
		m_r_audio(audioo),
		m_r_config(config)
	{}

	void initialize(ecs::state& state) {
		_current_selection = 0;

		_seeing_message = false;
		current_page = 0;
		how_to_page = 0;
		credits_page = 0;
		codex_page = 0;
		_current_options_selection = 0;
		_current_warning_selection = 0;
	}

	void update(ecs::state& state) override
	{
		auto e = state.first<components::pause>();
		auto a = state.first<components::pause_arrow>();

		if (e == nullptr || a == nullptr)
		{
			return;
		}

		auto& pause = e->get_component<components::pause>();
		auto& menu_audio = e->get_component<audio::audio_emitter>();

		if (!pause.is_game_started || !pause.is_game_countdown_over || pause.is_game_over)
		{
			return;
		}

		auto& renderable = e->get_component<rendering::renderable_mesh_static>();
		auto& arrow_renderable = a->get_component<rendering::renderable_mesh_static>();
		auto& arrow_transform = a->get_component<transforms::transform>();

		// Handle the popup screens instead of the pause menu
		if (_seeing_message) {
			if (_current_selection == HOW_TO) {
				handle_howto_case(state);
			}
			else if (_current_selection == CREDITS) {
				handle_credits_case(state);
			}
			else if (_current_selection == OPTIONS) {
				handle_options_case(state, arrow_transform, renderable, false);
			}
			else if (_current_selection == MAIN_MENU || _current_selection == REPLAY || _current_selection == QUIT) {
				confirm_destructive_action(state, arrow_transform, arrow_renderable, renderable, pause);
			}
			return;
		}

		if (m_r_glfw.is_minimized())
		{
			pause.is_game_paused = true;
			renderable.is_enabled = true;
			arrow_renderable.is_enabled = true;
		}

		// Toggle pause
		if (m_r_input.is_input_started(core::controls::MENU_CONTROL))
		{
			pause.is_game_paused = !pause.is_game_paused;
			renderable.is_enabled = !renderable.is_enabled;
			arrow_renderable.is_enabled = !arrow_renderable.is_enabled;
			menu_audio.set_sound_state(0, audio::sound_state::playback_requested);
		}

		if (pause.is_game_paused)
		{
			// Change selected
			if (m_r_input.is_input_started(core::controls::UP_CONTROL) || m_r_input.is_input_started(core::controls::UP_CONTROL_PLAYER2)) 
			{
				if (_current_selection == 0) {
					_current_selection = NUM_PAUSE_OPTIONS;
				}
				_current_selection--;
				arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
				arrow_transform.is_matrix_dirty = true;
				menu_audio.set_sound_state(0, audio::sound_state::playback_requested);
			}
			else if (m_r_input.is_input_started(core::controls::DOWN_CONTROL) || m_r_input.is_input_started(core::controls::DOWN_CONTROL_PLAYER2))
			{
				_current_selection = (++_current_selection) % NUM_PAUSE_OPTIONS;
				arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
				arrow_transform.is_matrix_dirty = true;
				menu_audio.set_sound_state(0, audio::sound_state::playback_requested);
			}

				// Choose the selected option
				else if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2)) 
			{
				menu_audio.set_sound_state(0, audio::sound_state::playback_requested);
				// Note: I tried this with a switch statement but it ended up being messier because of bypassing initialization of variables
				if (_current_selection == RESUME) {
					// Resume
					pause.is_game_paused = false;
					renderable.is_enabled = false;

					arrow_renderable.is_enabled = false;
				}
				else if (_current_selection == REPLAY) {
					// Replay
					_seeing_message = true;
					_current_warning_selection = 0;
				}
				else if (_current_selection == MAIN_MENU) {
					// Main Menu
					_seeing_message = true;
					_current_warning_selection = 0;
				}
				else if (_current_selection == QUIT) {
					// Quit
					_seeing_message = true;
					_current_warning_selection = 0;
				}
				else if (_current_selection == HOW_TO) {
					// How To
					_seeing_message = true;
					_current_warning_selection = 0;
				}
				else if (_current_selection == CREDITS) {
					// credits
					_seeing_message = true;
				}
				else if (_current_selection == OPTIONS) {
					// Options
					_seeing_message = true;
					_current_options_selection = 0;
					arrow_transform.position = glm::vec3(-27, 8, -5);
					arrow_transform.is_matrix_dirty = true;
					handle_options_case(state, arrow_transform, renderable, true);
				}
			}
		}
	}

	void handle_howto_case(ecs::state& state)
	{
		handle_pages(state, how_to_play_images, 3);
	}
	void handle_credits_case(ecs::state& state)
	{
		handle_pages(state, credits_images, 3);		
	}
	
	void handle_codex_case(ecs::state& state)
	{
		handle_pages(state, codex_images, 3);
	}
	void handle_pages(ecs::state& r_state, const entity_id pages[], int last_page) {
		for (size_t i = 0; i < 4; ++i)
		{
			auto& e = r_state.find_entity(pages[i]);
			auto& r = e.get_component<rendering::renderable_mesh_static>();
			if (i == current_page) {
				r.is_enabled = true;
			}
			else {
				r.is_enabled = false;
			}
		}
		if (m_r_input.any_button_pressed())
		{
			r_state.each_id<components::pause, audio::audio_emitter>([&](
				entity_id id1,
				components::pause& pause,
				audio::audio_emitter& emitter)
				{
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
				});
			current_page++;
			if (current_page > 3) {
				auto& e = r_state.find_entity(pages[last_page]);
				auto& r = e.get_component<rendering::renderable_mesh_static>();
				r.is_enabled = false;
				current_page = 0;
				_seeing_message = false;
			}
		}
	}

	enum options_choices {
		FULLSCREEN,
		MUTE_ALL,
		MUTE_MUSIC,
		BACK,
		NUM_OPTIONS
	};

	void handle_options_case(ecs::state& state, transforms::transform& arrow_transform, rendering::renderable_mesh_static& pause_renderable, bool is_new_opened) {
		auto& e = state.find_entity(108); // Options image
		auto& r = e.get_component<rendering::renderable_mesh_static>();
		r.is_enabled = true;
		pause_renderable.is_enabled = false;

		if (is_new_opened)
		{
			_current_options_selection = 0;
			arrow_transform.position = glm::vec3(-27 - _current_options_selection * 0.7, 8 - _current_options_selection * 0.9, -5);
			arrow_transform.is_matrix_dirty = true;
			return;
		}

		// Change selected
		if (m_r_input.is_input_started(core::controls::UP_CONTROL) || m_r_input.is_input_started(core::controls::UP_CONTROL_PLAYER2))
		{
			if (_current_options_selection == 0) {
				_current_options_selection = NUM_OPTIONS;
			}
			_current_options_selection--;
			arrow_transform.position = glm::vec3(-27 - _current_options_selection * 0.7, 8 - _current_options_selection * 0.9, -5);
			arrow_transform.is_matrix_dirty = true;
			state.each_id<components::pause, audio::audio_emitter>([&](
				entity_id id1,
				components::pause& pause,
				audio::audio_emitter& emitter)
				{
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
				});
		}
		else if (m_r_input.is_input_started(core::controls::DOWN_CONTROL) || m_r_input.is_input_started(core::controls::DOWN_CONTROL_PLAYER2))
		{
			_current_options_selection = (++_current_options_selection) % NUM_OPTIONS;
			arrow_transform.position = glm::vec3(-27 - _current_options_selection * 0.7, 8 - _current_options_selection * 0.9, -5);
			arrow_transform.is_matrix_dirty = true;
			state.each_id<components::pause, audio::audio_emitter>([&](
				entity_id id1,
				components::pause& pause,
				audio::audio_emitter& emitter)
				{
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
				});
		}
		// Choose selected
		else if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2)) {
			state.each_id<components::pause, audio::audio_emitter>([&](
				entity_id id1,
				components::pause& pause,
				audio::audio_emitter& emitter)
				{
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
				});
			if (_current_options_selection == FULLSCREEN) {
				// Fullscreen
				m_r_glfw.set_fullscreen(!m_r_glfw.is_fullscreen());
				m_r_config.set("fullscreen", !m_r_config.fullscreen());
			}
			else if (_current_options_selection == MUTE_ALL) {				
				auto any_sound = m_r_config.music_volume() != 0 || m_r_config.sfx_volume() != 0;				
				auto val = any_sound ? 0.f : core::startup_config::DefaultVolume;
				m_r_config.set("music_volume", val);
				m_r_config.set("sfx_volume", val);
			}
			else if (_current_options_selection == MUTE_MUSIC) {
				auto any_sound = m_r_config.music_volume() != 0;
				auto val = any_sound ? 0.f : core::startup_config::DefaultVolume;
				m_r_config.set("music_volume", any_sound);				
			}
			else if (_current_options_selection == BACK) {
				_seeing_message = false;
				r.is_enabled = false;
				pause_renderable.is_enabled = true;
				arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
				arrow_transform.is_matrix_dirty = true;
			}
		}
		else if (m_r_input.is_input_started(core::controls::BACK_CONTROL))
		{
			state.each_id<components::pause, audio::audio_emitter>([&](
				entity_id id1,
				components::pause& pause,
				audio::audio_emitter& emitter)
				{
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
				});
			_seeing_message = false;
			r.is_enabled = false;
			pause_renderable.is_enabled = true;
			arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
			arrow_transform.is_matrix_dirty = true;
		}
	}

	void confirm_destructive_action(ecs::state& state, transforms::transform& arrow_transform, rendering::renderable_mesh_static& arrow_renderable, rendering::renderable_mesh_static& pause_renderable, components::pause pause) {
		auto& e = state.find_entity(109); // Warning image
		auto& r = e.get_component<rendering::renderable_mesh_static>();
		r.is_enabled = true;
		pause_renderable.is_enabled = false;
		arrow_transform.position = glm::vec3(-27 - (_current_warning_selection + 2) * 0.7, 8 - (_current_warning_selection + 2) * 0.9, -3);
		arrow_transform.is_matrix_dirty = true;

		// Change selected
		if (m_r_input.is_input_started(core::controls::UP_CONTROL) || m_r_input.is_input_started(core::controls::UP_CONTROL_PLAYER2))
		{
			if (_current_warning_selection == 0) {
				_current_warning_selection = 2;
			}
			_current_warning_selection--;
			state.each_id<components::pause, audio::audio_emitter>([&](
				entity_id id1,
				components::pause& pause,
				audio::audio_emitter& emitter)
				{
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
				});
		}
		else if (m_r_input.is_input_started(core::controls::DOWN_CONTROL) || m_r_input.is_input_started(core::controls::DOWN_CONTROL_PLAYER2))
		{
			_current_warning_selection = (++_current_warning_selection) % 2;
			state.each_id<components::pause, audio::audio_emitter>([&](
				entity_id id1,
				components::pause& pause,
				audio::audio_emitter& emitter)
				{
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
				});
		}
		// Choose selected
		else if (m_r_input.is_input_started(core::controls::CARD1_CONTROL) || m_r_input.is_input_started(core::controls::CARD1_CONTROL_PLAYER2)) {
			state.each_id<components::pause, audio::audio_emitter>([&](
				entity_id id1,
				components::pause& pause,
				audio::audio_emitter& emitter)
				{
					emitter.set_sound_state(0, audio::sound_state::playback_requested);
				});
			if (_current_warning_selection == 0) {
				// Return to pause menu
				_seeing_message = false;
				r.is_enabled = false;
				pause_renderable.is_enabled = true;
				arrow_transform.position = glm::vec3(-27 - _current_selection * 0.7, 8 - _current_selection * 0.9, -5);
				arrow_transform.is_matrix_dirty = true;
			}
			else if (_current_warning_selection == 1) {
				// Confirm action
				if (_current_selection == QUIT) {
					m_r_glfw.set_should_close(true);
				}
				else if (_current_selection == MAIN_MENU) {
					pause.is_game_paused = false;
					pause_renderable.is_enabled = false;
					arrow_renderable.is_enabled = false;

					auto camera_entity = state.first<rendering::camera>();
					auto& music = camera_entity->get_component<audio::music_player>();
					music.set_sound_state(0, audio::sound_state::stop_requested);

					asset::scene_change_event restart_event("assets/scenes/main_menu.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
				else if (_current_selection == REPLAY) {
					pause.is_game_paused = false;
					pause_renderable.is_enabled = false;
					arrow_renderable.is_enabled = false;

					auto camera_entity = state.first<rendering::camera>();
					auto& music = camera_entity->get_component<audio::music_player>();
					music.set_sound_state(0, audio::sound_state::stop_requested);

					asset::scene_change_event restart_event("assets/scenes/scene.json");
					m_r_events.BroadcastEvent(restart_event);
					return;
				}
			}
		}
	}

private:
	core::frame_timer& m_r_timer;
	core::game_input_manager& m_r_input;
	core::glfw_context& m_r_glfw;
	asset::scene_hydrater& m_r_hydrater;
	event::EventManager& m_r_events;
	audio::audio_system& m_r_audio;
	core::startup_config& m_r_config;
	int _current_selection;

	bool _seeing_message;
	int current_page;
	int how_to_page;
	int credits_page;
	int codex_page;
	const entity_id how_to_play_images[4]{ 104, 105, 106, 107 };
	const entity_id credits_images[4]{ 110, 111, 112, 113 };
	const entity_id codex_images[4]{ 114, 115, 116, 117 };
	int _current_options_selection;
	int _current_warning_selection;
};

#endif
