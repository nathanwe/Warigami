#ifndef __STARTUP_H_
#define __STARTUP_H_

#include "core/startup_config.hpp"

namespace core
{
class startup
{
private:
    startup_config config;

public:
    const startup_config &get_config() { return config; }
};
} // namespace core

#endif
