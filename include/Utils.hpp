#pragma once
#include <SDL3/SDL.h>

namespace Utils
{
	inline float hireTimeInSeconds()
	{
		float t = SDL_GetTicks();
		t *= 0.001f;

		return t;
	}
}