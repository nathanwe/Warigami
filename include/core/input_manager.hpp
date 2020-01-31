//
// Created by sava on 10/16/19.
//

#ifndef __INPUT_MANAGER_HPP_
#define __INPUT_MANAGER_HPP_


#include <cstdint>
#include <cstring>

#include <util/running_average.hpp>

#include <glm/vec2.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utility>

#include <Windows.h>
#include <Xinput.h>

namespace core
{
    // Gamepad based on this tutorial: https://lcmccauley.wordpress.com/2014/01/05/gamepad-input-tutorial/
    // Gamepad buttons
    static const WORD XINPUT_buttons[] = {
        XINPUT_GAMEPAD_A,
        XINPUT_GAMEPAD_B,
        XINPUT_GAMEPAD_X,
        XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_DPAD_UP,
        XINPUT_GAMEPAD_DPAD_DOWN,
        XINPUT_GAMEPAD_DPAD_LEFT,
        XINPUT_GAMEPAD_DPAD_RIGHT,
        XINPUT_GAMEPAD_LEFT_SHOULDER,
        XINPUT_GAMEPAD_RIGHT_SHOULDER,
        XINPUT_GAMEPAD_LEFT_THUMB,
        XINPUT_GAMEPAD_RIGHT_THUMB,
        XINPUT_GAMEPAD_START,
        XINPUT_GAMEPAD_BACK
    };

    struct XINPUT_button_ids {
        XINPUT_button_ids();
        int A, B, X, Y; // Xbox buttons
        int pad_up, pad_down, pad_left, pad_right;
        int Lbumper, Rbumper;
        int Lstick_button, Rstick_button; // Stick press
        int start_button, back_button;
    };
    
    class gamepad {
    public:
        gamepad();
        gamepad(int gamepad_index);

        void update();
        void refresh_state();

        XINPUT_STATE get_state();
        int get_index();
        bool connected();

        // Thumbsticks
        bool Lstick_deadzone();
        bool Rstick_deadzone();
        // pair of <x, y>
        std::pair<short, short> Lstick_position();
        std::pair<short, short> Rstick_position();

        // Triggers
        // Values from 0.0 to 1.0
        float Ltrigger();
        float Rtrigger();

        bool is_button_pressed(int button);
        bool is_button_down(int button);

        // Vibration
        // Values from 0.0 to 1.0
        void rumble(float left_rumble = 0.0f, float right_rumble = 0.0f);

    private:
        XINPUT_STATE _state;
        int _index;

        static const int _button_count = 14; // Number of buttons in gamepad
        bool _prev_button_states[_button_count];
        bool _current_button_states[_button_count];
        bool _buttons_pressed[_button_count]; // Buttons being pressed/released in current frame
    };

    extern XINPUT_button_ids Xbuttons;

    //////////


    class input_manager
    {
        std::uint8_t _glfw_mouse_button_codes[16];
        std::uint16_t _glfw_key_codes[128];


    public:
        input_manager(GLFWwindow *window);

        bool is_key_down(std::uint16_t key);

        bool was_key_pressed(std::uint16_t key);

        bool was_key_released(std::uint16_t key);

        bool was_mouse_clicked(std::uint8_t mouse_button);

        bool was_mouse_released(std::uint8_t mouse_button);

        void add_gamepad();

        void update();

        const glm::vec2 mouse_delta() { return _mouse_delta.average(); }

    private:
        GLFWwindow *_window;
        double _last_x, _last_y;
        util::running_average<glm::vec2, float> _mouse_delta;

        std::uint8_t* _current_key_states;
        std::uint8_t* _last_key_states;

        std::uint8_t* _current_mouse_button_states;
        std::uint8_t* _last_mouse_button_states;

        std::uint8_t _key_state_buffer_a[GLFW_KEY_LAST];
        std::uint8_t _key_state_buffer_b[GLFW_KEY_LAST];
        std::uint8_t _mouse_button_state_a[GLFW_MOUSE_BUTTON_8 +1];
        std::uint8_t _mouse_button_state_b[GLFW_MOUSE_BUTTON_8 +1];

        glm::vec2 _last_click_position;

        static void (*build_mouse_callback(input_manager& r))(GLFWwindow* window, int, int, int)
        {
            static input_manager& manager = r;

            void (*callback)(GLFWwindow*, int, int, int) = ([](GLFWwindow* window, int button, int action, int mods) {
                (void)button;
                (void)action;
                (void)mods; // silence unused param warnings

                double x, y;
                glfwGetCursorPos(window, &x, &y);
                manager._last_click_position = {(float)x, (float)y};
            });

            return callback;
        }

        std::vector<gamepad*> _gamepads;
    };
}

#endif //__INPUT_MANAGER_HPP_
