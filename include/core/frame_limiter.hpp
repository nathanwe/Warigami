#ifndef __FRAME_LIMITER_H_
#define __FRAME_LIMITER_H_

#include <stdint.h>

#include <core/frame_timer.hpp>
#include <core/framerate.hpp>

namespace core
{
class frame_limiter
{

public:
    frame_limiter(
        const frame_timer &frame_timer,
        std::uint32_t target_framerate) : _frame_timer(frame_timer),
                                          _framerate(core::framerate(target_framerate))
    {
    }

    void wait_remainder() const;

private:
    const frame_timer &_frame_timer;
    core::framerate _framerate;
};
} // namespace core

#endif
