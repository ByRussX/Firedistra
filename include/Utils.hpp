#pragma once
#include <SDL3/SDL.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

namespace Utils
{
	inline float hireTimeInSeconds()
	{
		float t = SDL_GetTicks();
		t *= 0.001f;

		return t;
	}
}