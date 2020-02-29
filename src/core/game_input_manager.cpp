#include "core/game_input_manager.hpp"
#include "core/input_manager.hpp"

// We can rename these when we know our game controls
// This can also help us in the future if we want to give the option of changing keys/buttons
#define MENU_KEY		GLFW_KEY_M
#define BACK_KEY		GLFW_KEY_ESCAPE
#define CONFIRM_KEY		GLFW_KEY_ENTER

#define ACTION1_KEY		GLFW_KEY_SPACE
#define ACTION2_KEY		GLFW_KEY_Z
#define ACTION3_KEY		GLFW_KEY_X
#define ACTION4_KEY		GLFW_KEY_C

#define ACTION1_BUTTON		Xbuttons.A
#define ACTION2_BUTTON		Xbuttons.B
#define ACTION3_BUTTON		Xbuttons.X
#define ACTION4_BUTTON		Xbuttons.Y

// Demo controls
#define DESCEND_KEY		GLFW_KEY_LEFT_CONTROL
#define ASCEND_KEY		GLFW_KEY_SPACE
#define INTERACT_KEY	GLFW_KEY_E

#define INTERACT_BUTTON Xbuttons.A

// A lower number is a higher sensitivity
#define MOUSE_SENSITIVITY 32.f


core::XINPUT_button_ids Xbuttons;

core::game_input_manager::game_input_manager(input_manager input) : _input (input) {
	for (int i = 0; i < CONTROL_COUNT; i++) {
		_prev_game_state[i] = false;
		_current_game_state[i] = false;
	}
}

void core::game_input_manager::update() {
	_input.update();

	memcpy(_prev_game_state, _current_game_state, sizeof(_prev_game_state));

	// Update pitch and yaw axis states
	_pitch = _input.get_gamepad()->engaged_Rstick_position().second;
	_yaw = _input.get_gamepad()->engaged_Rstick_position().first;

	auto mousedt = _input.mouse_delta();
	mousedt /= MOUSE_SENSITIVITY;
	_pitch += mousedt.y;
	_yaw += mousedt.x;

	// Update forward and strafe axis states
	_forward = _input.get_gamepad()->engaged_Lstick_position().second;
	_strafe = _input.get_gamepad()->engaged_Lstick_position().first;

	if (_input.is_key_down(GLFW_KEY_W) || _input.is_key_down(GLFW_KEY_UP)) {
		_forward = -1.0f;
	} else if (_input.is_key_down(GLFW_KEY_S) || _input.is_key_down(GLFW_KEY_DOWN)) {
		_forward = 1.0f;
	}

	if (_input.is_key_down(GLFW_KEY_A) || _input.is_key_down(GLFW_KEY_LEFT)) {
		_strafe = -1.0f;
	} else if (_input.is_key_down(GLFW_KEY_D) || _input.is_key_down(GLFW_KEY_RIGHT)) {
		_strafe = 1.0f;
	}

	// Check axis inputs against epsilon for state
	// UP
	//// W, up key, or left stick up
	_current_game_state[UP_CONTROL] = _forward < -THUMBPAD_EPSILON;

	// DOWN
	//// S, down key, or left stick down
	_current_game_state[DOWN_CONTROL] = _forward > THUMBPAD_EPSILON;

	// LEFT
	//// A, left key, or left stick left
	_current_game_state[LEFT_CONTROL] = _strafe < -THUMBPAD_EPSILON;

	// RIGHT
	//// D, right key, or left stick right
	_current_game_state[RIGHT_CONTROL] = _strafe > THUMBPAD_EPSILON;

	// MENU
	_current_game_state[MENU_CONTROL] = _input.is_key_down(MENU_KEY)
		|| _input.get_gamepad()->is_button_held(Xbuttons.start_button);

	// BACK
	_current_game_state[BACK_CONTROL] = _input.is_key_down(BACK_KEY)
		|| _input.get_gamepad()->is_button_held(Xbuttons.B);

	// CONFIRM
	_current_game_state[CONFIRM_CONTROL] = _input.is_key_down(CONFIRM_KEY)
		|| _input.get_gamepad()->is_button_held(Xbuttons.A);

	// ACTION 1
	_current_game_state[ACTION1_CONTROL] = _input.is_key_down(ACTION1_KEY)
		|| _input.get_gamepad()->is_button_held(ACTION1_BUTTON);

	// ACTION 2
	_current_game_state[ACTION2_CONTROL] = _input.is_key_down(ACTION2_KEY)
		|| _input.get_gamepad()->is_button_held(ACTION2_BUTTON);

	// ACTION 3
	_current_game_state[ACTION3_CONTROL] = _input.is_key_down(ACTION3_KEY)
		|| _input.get_gamepad()->is_button_held(ACTION3_BUTTON);

	// ACTION 4
	_current_game_state[ACTION4_CONTROL] = _input.is_key_down(ACTION4_KEY)
		|| _input.get_gamepad()->is_button_held(ACTION4_BUTTON);

	// INTERACT
	_current_game_state[INTERACT_CONTROL] = _input.is_key_down(INTERACT_KEY)
		|| _input.get_gamepad()->is_button_held(INTERACT_BUTTON);

	// DESCEND
	// Right stick down and DESCEND_KEY
	_current_game_state[DESCEND_CONTROL] = _input.is_key_down(DESCEND_KEY)
		|| (_input.get_gamepad()->Rstick_position().second < 0 && !_input.get_gamepad()->Rstick_deadzone());

	// ASCEND
	// Right stick up and ASCEND_KEY
	_current_game_state[ASCEND_CONTROL] = _input.is_key_down(ASCEND_KEY)
		|| (_input.get_gamepad()->Rstick_position().second > 0 && !_input.get_gamepad()->Rstick_deadzone());

	// DEVELOPER CONSOLE
	// ~ on keyboard (no gamepad)
	_current_game_state[DEVELOPER_CONSOLE] = _input.is_key_down(GLFW_KEY_GRAVE_ACCENT);
}

bool core::game_input_manager::is_input_active(controls control) {
	return _current_game_state[control];
}

bool core::game_input_manager::is_input_started(controls control) {
	return _current_game_state[control] && !_prev_game_state[control];
}

bool core::game_input_manager::is_input_ended(controls control) {
	return !_current_game_state[control] && _prev_game_state[control];
}