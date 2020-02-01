#ifndef __RUMBLE_HPP_
#define __RUMBLE_HPP_

#include <cstdint>

namespace os
{
	void rumble(std::uint32_t index, float left, float right);
}

#endif