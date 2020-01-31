//#include "core/input_event_manager.hpp"
//#include "core/input_manager.hpp"
//
//void core::input_event_manager::update() {
//	// A, up key, or left stick up
//	_current_game_state[UP_EVENT] = is_key_down(GLFW_KEY_A) || is_key_down(GLFW_KEY_UP) 
//		|| (Lstick_position().second() > 0 && !Lstick_deadzone);
//}
//
//bool core::input_event_manager::is_input_active(input_events input_event) {
//	return _current_game_state[input_event];
//}
//
//bool core::input_event_manager::is_input_started(input_events input_event) {
//	return _current_game_state[input_event] && !_prev_game_state[input_event];
//}
//
//bool core::input_event_manager::is_input_ended(input_events input_event) {
//	return !_current_game_state[input_event] && _prev_game_state[input_event];
//}