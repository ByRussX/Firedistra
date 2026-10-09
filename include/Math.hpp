#pragma once
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class Vector2f
{
public:
	float x, y;
	
	inline Vector2f(float p_x = 0.0f, float p_y = 0.0f): x(p_x), y(p_y) {}

	inline void print()
	{
		std::cout << x << ", " << y << std::endl;
	}
};