#ifndef __SLEEP_H_
#define __SLEEP_H_

#include <chrono>
#include <stdint.h>

namespace os
{

void sleep(std::chrono::nanoseconds millis);

} // namespace os

#endif
