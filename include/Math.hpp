#pragma once
#include <iostream>
#include <cmath>

class Vector2f
{
public:
    float x;
    float y;
    
    explicit constexpr Vector2f(float p_x = 0.0f, float p_y = 0.0f) : x(p_x), y(p_y) {}

    // Operator overloading
    constexpr Vector2f operator+(Vector2f other) const
    {
        return Vector2f(x + other.x, y + other.y);
    }

    constexpr Vector2f operator-(Vector2f other) const
    {
        return Vector2f(x - other.x, y - other.y);
    }

    constexpr Vector2f operator-() const
    {
        return Vector2f(-x, -y);
    }

    constexpr Vector2f operator*(float scalar) const
    {
        return Vector2f(x * scalar, y * scalar);
    }

    friend constexpr Vector2f operator*(float scalar, Vector2f vec)
    {
        return Vector2f(vec.x * scalar, vec.y * scalar);
    }

    constexpr Vector2f operator/(float scalar) const
    {
        return Vector2f(x / scalar, y / scalar);
    }

    Vector2f& operator+=(Vector2f other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    Vector2f& operator-=(Vector2f other)
    {
        x -= other.x;
        y -= other.y;
        

        return *this;
    }

    Vector2f& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2f& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    constexpr bool operator==(Vector2f other) const
    {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(Vector2f other) const
    {
        return !(*this == other);
    }

    // Vector length
    float length() const
    {
        return std::sqrt(x * x + y * y);
    }

    // Normalized vector
    Vector2f normalized() const
    {
        float len = length();
        if (len > 0.0f)
        {
            float invLen = 1.0f / len;
            return Vector2f(x * invLen, y * invLen);
        }
        return Vector2f(0.0f, 0.0f);
    }

    // Dot product
    constexpr float dot(Vector2f other) const
    {
        return x * other.x + y * other.y;
    }

    // 2D Cross product of two vectors (returns scalar z component)
    constexpr float cross(Vector2f other) const
    {
        return x * other.y - y * other.x;
    }

    static constexpr float cross(Vector2f a, Vector2f b)
    {
        return a.x * b.y - a.y * b.x;
    }

    // 2D Cross product: scalar (z) x vector -> vector: (-s * v.y, s * v.x)
    static constexpr Vector2f cross(float s, Vector2f v)
    {
        return Vector2f(-s * v.y, s * v.x);
    }

    // 2D Cross product: vector x scalar (z) -> vector: (s * v.y, -s * v.x)
    static constexpr Vector2f cross(Vector2f v, float s)
    {
        return Vector2f(s * v.y, -s * v.x);
    }

    // Perpendicular vector
    constexpr Vector2f perp() const
    {
        return Vector2f(-y, x);
    }

    // Rotate the vector by an angle in radians
    Vector2f rotate(float angleRad) const
    {
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        return Vector2f(x * c - y * s, x * s + y * c);
    }

    void print() const
    {
        std::cout << x << ", " << y << std::endl;
    }
};

namespace Math
{
    constexpr float PI = 3.14159265358979323846f;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;
}