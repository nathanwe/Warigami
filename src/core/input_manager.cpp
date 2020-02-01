//
// Created by sava on 10/16/19.
// Edited by Silvia Barbero 1/29/20
//

#include "core/input_manager.hpp"

#include <iostream>

//core::XINPUT_button_ids Xbuttons;

core::XINPUT_button_ids::XINPUT_button_ids() {
    A = GLFW_GAMEPAD_BUTTON_A; 
    B = GLFW_GAMEPAD_BUTTON_B; 
    X = GLFW_GAMEPAD_BUTTON_X; 
    Y = GLFW_GAMEPAD_BUTTON_Y;
    pad_up = GLFW_GAMEPAD_BUTTON_DPAD_UP; 
    pad_down = GLFW_GAMEPAD_BUTTON_DPAD_DOWN; 
    pad_left = GLFW_GAMEPAD_BUTTON_DPAD_LEFT; 
    pad_right = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
    Lbumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
    Rbumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
    Lstick_button = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
    Rstick_button = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
    start_button = GLFW_GAMEPAD_BUTTON_START; 
    back_button = GLFW_GAMEPAD_BUTTON_BACK;
}

core::gamepad::gamepad(int index) 
    : _index(index) 
    , _connected(glfwJoystickPresent(index)){
    for (int i = 0; i < _button_count; i++) {
        _prev_button_states[i] = false;
        _current_button_states[i] = false;
    }
}

GLFWgamepadstate core::gamepad::get_state() {
    auto joystick_num = GLFW_JOYSTICK_1 + _index;
    GLFWgamepadstate state;
    glfwGetGamepadState(joystick_num, &state);
    return state; 

    /*XINPUT_STATE current_state;
    memset(&current_state, 0, sizeof(XINPUT_STATE));
    XInputGetState(_index, &current_state);
    return current_state;*/
}

int core::gamepad::get_index() {
    return _index;
}

bool core::gamepad::connected() {
    auto joystick_num = GLFW_JOYSTICK_1 + _index;
    return  glfwJoystickPresent(joystick_num);
}

void core::gamepad::update() {
    _connected = glfwJoystickPresent(_index);

    if (_connected)
    {
        memcpy(_prev_button_states, _current_button_states, sizeof(_prev_button_states));
        _state = get_state();
    }
}

bool core::gamepad::Lstick_deadzone() {
    auto x = _state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    auto y = _state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

    if (x > THUMBPAD_EPSILON || x < -THUMBPAD_EPSILON) {
        return false;
    }

    if (y > THUMBPAD_EPSILON || y < -THUMBPAD_EPSILON) {
        return false;
    }

    return true;
}

bool core::gamepad::Rstick_deadzone() {
    auto x = _state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    auto y = _state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

    if (x > THUMBPAD_EPSILON || x < -THUMBPAD_EPSILON) {
        return false;
    }

    if (y > THUMBPAD_EPSILON || y < -THUMBPAD_EPSILON) {
        return false;
    }

    return true;
}

std::pair<float, float> core::gamepad::Lstick_position() {
    return { _state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], _state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] };
}

std::pair<float, float> core::gamepad::Rstick_position() {
    return { _state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], _state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] };
}

float core::gamepad::Ltrigger() {
    auto trigger_value = _state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
    if (trigger_value > THUMBPAD_EPSILON || trigger_value < -THUMBPAD_EPSILON) {
        // Value goes from -1 to 1, transform from 0.0 to 1.0
        return (trigger_value + 1.f) / 2.f;
    }
    return 0.0f;
}

float core::gamepad::Rtrigger() {
    auto trigger_value = _state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
    if (trigger_value > THUMBPAD_EPSILON || trigger_value < -THUMBPAD_EPSILON) {
        // Value goes from -1 to 1, transform from 0.0 to 1.0
        return (trigger_value + 1.f) / 2.f;
    }
    return 0.0f;
}

void core::gamepad::rumble(float left_rumble, float right_rumble) {
    os::rumble(_index, left_rumble, right_rumble);
}

bool core::gamepad::is_button_held(int button) {
    return _current_button_states[button];
}

bool core::gamepad::is_button_pressed(int button) {
    return _current_button_states[button] && !_prev_button_states[button];
}

bool core::gamepad::is_button_released(int button) {
    return !_current_button_states[button] && _prev_button_states[button];
}

/////////////

bool core::input_manager::is_key_down(std::uint16_t key)
{
    return _current_key_states[key];
}

bool core::input_manager::was_key_pressed(std::uint16_t key)
{
    return !(_last_key_states[key]) &&
        _current_key_states[key];
}

bool core::input_manager::was_key_released(std::uint16_t key)
{
    return _last_key_states[key] != GLFW_RELEASE &&
        _current_key_states[key] == GLFW_RELEASE;
}

bool core::input_manager::was_mouse_clicked(std::uint8_t mouse_button)
{
    return !(_last_mouse_button_states[mouse_button]) &&
        _current_mouse_button_states[mouse_button];
}

bool core::input_manager::was_mouse_released(std::uint8_t mouse_button)
{
    return _last_mouse_button_states[mouse_button] != GLFW_RELEASE &&
        _current_mouse_button_states[mouse_button] == GLFW_RELEASE;
}

void core::input_manager::update()
{    
    _last_key_states = _current_key_states;
    _last_mouse_button_states = _current_mouse_button_states;    

    for(int i = 0; _glfw_key_codes[i] != GLFW_KEY_LAST; ++i)
    {
        auto state = glfwGetKey(_window, _glfw_key_codes[i]);
        _current_key_states[_glfw_key_codes[i]] = (std::uint8_t) state;
    }

    for(int i = 0; _glfw_mouse_button_codes[i] != GLFW_MOUSE_BUTTON_LAST; ++i)
    {
        auto state = glfwGetMouseButton(_window, _glfw_mouse_button_codes[i]);
        _current_mouse_button_states[_glfw_mouse_button_codes[i]] = (std::uint8_t) state;
    }

    double x, y;
    glfwGetCursorPos(_window, &x, &y);

    _mouse_delta.update(glm::vec2(x - _last_x, y - _last_y));
    _last_x = x;
    _last_y = y;

    _gamepad->update();
}

core::input_manager::input_manager(GLFWwindow *window) :
    _window(window),
    _mouse_delta(3, glm::vec2(0)),
    _current_key_states(_key_state_buffer_a),
    _last_key_states(_key_state_buffer_b),
    _current_mouse_button_states(_mouse_button_state_a),
    _last_mouse_button_states(_mouse_button_state_b),
    _glfw_mouse_button_codes {
        GLFW_MOUSE_BUTTON_1,
        GLFW_MOUSE_BUTTON_2,
        GLFW_MOUSE_BUTTON_3,
        GLFW_MOUSE_BUTTON_4,
        GLFW_MOUSE_BUTTON_5,
        GLFW_MOUSE_BUTTON_6,
        GLFW_MOUSE_BUTTON_7,
        GLFW_MOUSE_BUTTON_8
    },
    _glfw_key_codes {
        GLFW_KEY_SPACE,
        GLFW_KEY_APOSTROPHE,
        GLFW_KEY_COMMA,
        GLFW_KEY_MINUS,
        GLFW_KEY_PERIOD,
        GLFW_KEY_SLASH,
        GLFW_KEY_0,
        GLFW_KEY_1,
        GLFW_KEY_2,
        GLFW_KEY_3,
        GLFW_KEY_4,
        GLFW_KEY_5,
        GLFW_KEY_6,
        GLFW_KEY_7,
        GLFW_KEY_8,
        GLFW_KEY_9,
        GLFW_KEY_SEMICOLON,
        GLFW_KEY_EQUAL,
        GLFW_KEY_A,
        GLFW_KEY_B,
        GLFW_KEY_C,
        GLFW_KEY_D,
        GLFW_KEY_E,
        GLFW_KEY_F,
        GLFW_KEY_G,
        GLFW_KEY_H,
        GLFW_KEY_I,
        GLFW_KEY_J,
        GLFW_KEY_K,
        GLFW_KEY_L,
        GLFW_KEY_M,
        GLFW_KEY_N,
        GLFW_KEY_O,
        GLFW_KEY_P,
        GLFW_KEY_Q,
        GLFW_KEY_R,
        GLFW_KEY_S,
        GLFW_KEY_T,
        GLFW_KEY_U,
        GLFW_KEY_V,
        GLFW_KEY_W,
        GLFW_KEY_X,
        GLFW_KEY_Y,
        GLFW_KEY_Z,
        GLFW_KEY_LEFT_BRACKET,
        GLFW_KEY_BACKSLASH,
        GLFW_KEY_RIGHT_BRACKET,
        GLFW_KEY_GRAVE_ACCENT,
        GLFW_KEY_WORLD_1,
        GLFW_KEY_WORLD_2,
        GLFW_KEY_ESCAPE,
        GLFW_KEY_ENTER,
        GLFW_KEY_TAB,
        GLFW_KEY_BACKSPACE,
        GLFW_KEY_INSERT,
        GLFW_KEY_DELETE,
        GLFW_KEY_RIGHT,
        GLFW_KEY_LEFT,
        GLFW_KEY_DOWN,
        GLFW_KEY_UP,
        GLFW_KEY_PAGE_UP,
        GLFW_KEY_PAGE_DOWN,
        GLFW_KEY_HOME,
        GLFW_KEY_END,
        GLFW_KEY_CAPS_LOCK,
        GLFW_KEY_SCROLL_LOCK,
        GLFW_KEY_NUM_LOCK,
        GLFW_KEY_PRINT_SCREEN,
        GLFW_KEY_PAUSE,
        GLFW_KEY_F1,
        GLFW_KEY_F2,
        GLFW_KEY_F3,
        GLFW_KEY_F4,
        GLFW_KEY_F5,
        GLFW_KEY_F6,
        GLFW_KEY_F7,
        GLFW_KEY_F8,
        GLFW_KEY_F9,
        GLFW_KEY_F10,
        GLFW_KEY_F11,
        GLFW_KEY_F12,
        GLFW_KEY_F13,
        GLFW_KEY_F14,
        GLFW_KEY_F15,
        GLFW_KEY_F16,
        GLFW_KEY_F17,
        GLFW_KEY_F18,
        GLFW_KEY_F19,
        GLFW_KEY_F20,
        GLFW_KEY_F21,
        GLFW_KEY_F22,
        GLFW_KEY_F23,
        GLFW_KEY_F24,
        GLFW_KEY_F25,
        GLFW_KEY_KP_0,
        GLFW_KEY_KP_1,
        GLFW_KEY_KP_2,
        GLFW_KEY_KP_3,
        GLFW_KEY_KP_4,
        GLFW_KEY_KP_5,
        GLFW_KEY_KP_6,
        GLFW_KEY_KP_7,
        GLFW_KEY_KP_8,
        GLFW_KEY_KP_9,
        GLFW_KEY_KP_DECIMAL,
        GLFW_KEY_KP_DIVIDE,
        GLFW_KEY_KP_MULTIPLY,
        GLFW_KEY_KP_SUBTRACT,
        GLFW_KEY_KP_ADD,
        GLFW_KEY_KP_ENTER,
        GLFW_KEY_KP_EQUAL,
        GLFW_KEY_LEFT_SHIFT,
        GLFW_KEY_LEFT_CONTROL,
        GLFW_KEY_LEFT_ALT,
        GLFW_KEY_LEFT_SUPER,
        GLFW_KEY_RIGHT_SHIFT,
        GLFW_KEY_RIGHT_CONTROL,
        GLFW_KEY_RIGHT_ALT,
        GLFW_KEY_RIGHT_SUPER,
        GLFW_KEY_MENU,
        GLFW_KEY_LAST
    }
{
    memset(_key_state_buffer_a, 0, sizeof(_key_state_buffer_a));
    memset(_key_state_buffer_b, 0, sizeof(_key_state_buffer_b));
    memset(_mouse_button_state_a, 0, sizeof(_mouse_button_state_a));
    memset(_mouse_button_state_b, 0, sizeof(_mouse_button_state_b));

    glfwSetMouseButtonCallback(window, build_mouse_callback(*this));

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    double x, y;
    glfwGetCursorPos(_window, &x, &y);
    _last_x = x;
    _last_y = y;
        
    _gamepad = new gamepad(0);
}

core::gamepad* core::input_manager::get_gamepad() {
    return _gamepad;
}
