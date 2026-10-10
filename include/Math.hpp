#pragma once
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cmath>

class Vector2f
{
public:
    float x, y;
    
    constexpr Vector2f(float p_x = 0.0f, float p_y = 0.0f) : x(p_x), y(p_y) {}

    // Operator overloading
    Vector2f operator+(const Vector2f & other) const
	{
		return Vector2f(x + other.x, y + other.y);
	
	}
    Vector2f operator-(const Vector2f & other) const
	{
		return Vector2f(x - other.x, y - other.y);
	
	}
    Vector2f operator*(const float scalar) const
	{
		return Vector2f(x * scalar, y * scalar);
	}

    Vector2f & operator+=(const Vector2f& other)
	{
		x += other.x; y += other.y;
		return * this;
	}

    float length() const
	{
		return std::sqrt(x * x + y * y);
	}

    Vector2f normalized() const
	{
        float len = length();
        return len > 0.0f ? Vector2f(x / len, y / len) : Vector2f(0.0f, 0.0f);
    }
};