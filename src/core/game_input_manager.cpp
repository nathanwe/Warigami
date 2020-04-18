#include "core/game_input_manager.hpp"
#include "core/input_manager.hpp"

// This can also help us in the future if we want to give the option of changing keys/buttons
// Shared inputs
#define MENU_KEY		GLFW_KEY_ESCAPE
#define BACK_KEY		GLFW_KEY_ESCAPE
#define CONFIRM_KEY		GLFW_KEY_ENTER

// Player 1 keyboard inputs
#define CARD1_KEY		GLFW_KEY_Z
#define CARD2_KEY		GLFW_KEY_X
#define CARD3_KEY		GLFW_KEY_C
#define CARD4_KEY		GLFW_KEY_V
#define DIE1_KEY		GLFW_KEY_F
#define DIE2_KEY		GLFW_KEY_G

// Player 2 keyboard inputs
#define CARD1_KEY_PLAYER2		GLFW_KEY_U
#define CARD2_KEY_PLAYER2		GLFW_KEY_I
#define CARD3_KEY_PLAYER2		GLFW_KEY_O
#define CARD4_KEY_PLAYER2		GLFW_KEY_P
#define DIE1_KEY_PLAYER2		GLFW_KEY_K
#define DIE2_KEY_PLAYER2		GLFW_KEY_L

// Controller inputs
#define CARD1_BUTTON		Xbuttons.A
#define CARD2_BUTTON		Xbuttons.B
#define CARD3_BUTTON		Xbuttons.X
#define CARD4_BUTTON		Xbuttons.Y
#define DIE1_BUTTON			Xbuttons.Lbumper
#define DIE2_BUTTON			Xbuttons.Rbumper
#define START_BUTTON		Xbuttons.start_button
#define BACK_BUTTON			Xbuttons.back_button

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
	_pitch = _input.get_gamepads()[0]->engaged_Rstick_position().second;
	_yaw = _input.get_gamepads()[0]->engaged_Rstick_position().first;

	_pitch_player2 = _input.get_gamepads()[1]->engaged_Rstick_position().second;
	_yaw_player2 = _input.get_gamepads()[1]->engaged_Rstick_position().first;

	auto mousedt = _input.mouse_delta();
	mousedt /= MOUSE_SENSITIVITY;
	_pitch += mousedt.y;
	_yaw += mousedt.x;

	// Update forward and strafe axis states
	_forward = _input.get_gamepads()[0]->engaged_Lstick_position().second;
	_strafe = _input.get_gamepads()[0]->engaged_Lstick_position().first;

	_forward_player2 = _input.get_gamepads()[1]->engaged_Lstick_position().second;
	_strafe_player2 = _input.get_gamepads()[1]->engaged_Lstick_position().first;

	if (_input.is_key_down(GLFW_KEY_W)) {
		_forward = -1.0f;
	}
	else if (_input.is_key_down(GLFW_KEY_S)) {
		_forward = 1.0f;
	}

	if (_input.is_key_down(GLFW_KEY_UP)) {
		_forward_player2 = -1.0f;
	}
	else if (_input.is_key_down(GLFW_KEY_DOWN)) {
		_forward_player2 = 1.0f;
	}

	if (_input.is_key_down(GLFW_KEY_A)) {
		_strafe = -1.0f;
	} else if (_input.is_key_down(GLFW_KEY_D)) {
		_strafe = 1.0f;
	}

	if ( _input.is_key_down(GLFW_KEY_LEFT)) {
		_strafe_player2 = -1.0f;
	}
	else if (_input.is_key_down(GLFW_KEY_RIGHT)) {
		_strafe_player2 = 1.0f;
	}

	_right_trigger = _input.get_gamepads()[0]->Rtrigger();
	_right_trigger_player2 = _input.get_gamepads()[1]->Rtrigger();
	if (_input.is_key_down(DIE2_KEY))
		_right_trigger = -1.f;
	if (_input.is_key_down(DIE2_KEY_PLAYER2))
		_right_trigger_player2 = 1.f;

	// Check axis inputs against epsilon for state
	// UP
	//// W, up key, or left stick up
	_current_game_state[UP_CONTROL] = _forward < -THUMBPAD_EPSILON;
	_current_game_state[UP_CONTROL_PLAYER2] = _forward_player2 < -THUMBPAD_EPSILON;

	// DOWN
	//// S, down key, or left stick down
	_current_game_state[DOWN_CONTROL] = _forward > THUMBPAD_EPSILON;
	_current_game_state[DOWN_CONTROL_PLAYER2] = _forward_player2 > THUMBPAD_EPSILON;

	// LEFT
	//// A, left key, or left stick left
	_current_game_state[LEFT_CONTROL] = _strafe < -THUMBPAD_EPSILON;
	_current_game_state[LEFT_CONTROL_PLAYER2] = _strafe_player2 < -THUMBPAD_EPSILON;

	// RIGHT
	//// D, right key, or left stick right
	_current_game_state[RIGHT_CONTROL] = _strafe > THUMBPAD_EPSILON;
	_current_game_state[RIGHT_CONTROL_PLAYER2] = _strafe_player2 > THUMBPAD_EPSILON;

	// MENU
	_current_game_state[MENU_CONTROL] = _input.is_key_down(MENU_KEY)
		|| _input.get_gamepads()[0]->is_button_held(Xbuttons.start_button);

	// BACK
	_current_game_state[BACK_CONTROL] = _input.is_key_down(BACK_KEY)
		|| _input.get_gamepads()[0]->is_button_held(Xbuttons.B);

	// CONFIRM
	_current_game_state[CONFIRM_CONTROL] = _input.is_key_down(CONFIRM_KEY)
		|| _input.get_gamepads()[0]->is_button_held(Xbuttons.A);

	// CARD 1
	_current_game_state[CARD1_CONTROL] = _input.is_key_down(CARD1_KEY)
		|| _input.get_gamepads()[0]->is_button_held(CARD1_BUTTON);
	_current_game_state[CARD1_CONTROL_PLAYER2] = _input.is_key_down(CARD1_KEY_PLAYER2)
		|| _input.get_gamepads()[1]->is_button_held(CARD1_BUTTON);

	// CARD 2
	_current_game_state[CARD2_CONTROL] = _input.is_key_down(CARD2_KEY)
		|| _input.get_gamepads()[0]->is_button_held(CARD2_BUTTON);
	_current_game_state[CARD2_CONTROL_PLAYER2] = _input.is_key_down(CARD2_KEY_PLAYER2)
		|| _input.get_gamepads()[1]->is_button_held(CARD2_BUTTON);

	// CARD 3
	_current_game_state[CARD3_CONTROL] = _input.is_key_down(CARD3_KEY)
		|| _input.get_gamepads()[0]->is_button_held(CARD3_BUTTON);
	_current_game_state[CARD3_CONTROL_PLAYER2] = _input.is_key_down(CARD3_KEY_PLAYER2)
		|| _input.get_gamepads()[1]->is_button_held(CARD3_BUTTON);

	// CARD 4
	_current_game_state[CARD4_CONTROL] = _input.is_key_down(CARD4_KEY)
		|| _input.get_gamepads()[0]->is_button_held(CARD4_BUTTON);
	_current_game_state[CARD4_CONTROL_PLAYER2] = _input.is_key_down(CARD4_KEY_PLAYER2)
		|| _input.get_gamepads()[1]->is_button_held(CARD4_BUTTON);

	// DIE 1
	_current_game_state[DIE1_CONTROL] = _input.is_key_down(DIE1_KEY)
		|| _input.get_gamepads()[0]->is_button_held(DIE1_BUTTON);
	_current_game_state[DIE1_CONTROL_PLAYER2] = _input.is_key_down(DIE1_KEY_PLAYER2)
		|| _input.get_gamepads()[1]->is_button_held(DIE1_BUTTON);

	// DIE 2
	_current_game_state[DIE2_CONTROL] = _input.is_key_down(DIE2_KEY)
		|| _input.get_gamepads()[0]->is_button_held(DIE2_BUTTON);

	_current_game_state[DIE2_CONTROL_PLAYER2] = _input.is_key_down(DIE2_KEY_PLAYER2)
		|| _input.get_gamepads()[1]->is_button_held(DIE2_BUTTON);


	_current_game_state[START] = _input.is_key_down(GLFW_KEY_SPACE) || _input.is_key_down(GLFW_KEY_ENTER) ||
		_input.get_gamepads()[0]->is_button_held(START_BUTTON);

	_current_game_state[BACK] = _input.is_key_down(GLFW_KEY_I) ||
		_input.get_gamepads()[0]->is_button_held(BACK_BUTTON);

	// DEVELOPER CONSOLE
	// ~ on keyboard (no gamepad)
	_current_game_state[DEVELOPER_CONSOLE] = _input.is_key_down(GLFW_KEY_GRAVE_ACCENT);

	_any_button_pressed = false;
	for (size_t i = 0; i < controls::CONTROL_COUNT; ++i)
	{
		_any_button_pressed = _any_button_pressed || is_input_started((controls)i);
	}
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