#include <platform/rumble.hpp>

#include <stdlib.h>
#include <cstring>

#ifdef WIN32
#include <Windows.h>
#include <Xinput.h>

void os::rumble(std::uint32_t index, float left, float right)
{
	XINPUT_VIBRATION vibration_state;
	memset(&vibration_state, 0, sizeof(XINPUT_VIBRATION));

	// Vibration ranges from 0 to 65535
	vibration_state.wLeftMotorSpeed = int(left * 65535.0f);
	vibration_state.wRightMotorSpeed = int(right * 65535.0f);

	XInputSetState(index, &vibration_state);
}
#endif

#ifdef __linux__
#include <linux/input.h>
#include <sys/ioctl.h>

void os::rumble(std::uint32_t index, float left, float right)
{
	struct ff_effect ff;
	//...
}

#endif