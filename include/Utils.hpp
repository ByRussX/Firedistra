#pragma once
#include <SDL3/SDL.h>

namespace Utils
{
    // Returns elapsed time in seconds
    inline double getTimeInSeconds()
    {
        return static_cast<double>(SDL_GetTicksNS()) * 1e-9;
    }
}