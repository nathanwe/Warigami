#include "platform/sleep.hpp"

#define SPINLOCK_SLEEP

// TODO: Implement high-precision sleep on various platforms, using
// high-precision sleep code specific to platform. Then turn off SPINLOCK_SLEEP See:
// https://stackoverflow.com/questions/13397571/precise-thread-sleep-needed-max-1ms-error
#ifdef SPINLOCK_SLEEP

void os::sleep(std::chrono::nanoseconds nanos)
{
    auto start = std::chrono::high_resolution_clock::now();

    while (true)
    {
        auto now = std::chrono::high_resolution_clock::now();
        if (now - start > nanos) return;
    }
}

#else



#if (WIN32)

#define NOMINMAX
#include <windows.h>

#include <timeapi.h>

void os::sleep(std::chrono::nanoseconds nanos)
{
    // TODO: use media timer
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(nanos).count();
	auto wait_time = (DWORD)(millis < 0 ? 0 : millis);
    Sleep(wait_time);
}

#elif (__linux__)
#include <time.h>

void os::sleep(std::chrono::nanoseconds nanos)
{
    timespec ts = {0, nanos.count()};
    nanosleep(&ts, NULL);
}

#else
// nanosleep should be available on mac in the same way?
#include <time.h>

void os::sleep(std::chrono::nanoseconds nanos)
{
    timespec ts = {0, nanos.count()};
    nanosleep(&ts, NULL);
}
#endif


#endif