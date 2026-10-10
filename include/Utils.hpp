#pragma once
#include <SDL3/SDL.h>

namespace Utils
{
    // Returns time in seconds with high precision using SDL3 nanoseconds timer
    inline double getTimeInSeconds()
    {
        return static_cast<double>(SDL_GetTicksNS()) * 1e-9;
    }

    // Alias for backwards compatibility
    inline double hireTimeInSeconds()
    {
        return getTimeInSeconds();
    }
}