#pragma once

#include <SDL3/SDL.h>
#include "Math.hpp"

class Hitbox
{
public:
    Hitbox();

    Vector2f getOffset() const;
    Vector2f getSize() const;

    void setOffset(const Vector2f & offset);
    void setSize(const Vector2f & size);

private:
    SDL_FRect hitbox{0.0f, 0.0f, 0.0f, 0.0f}; // Hitbox rectangle
};