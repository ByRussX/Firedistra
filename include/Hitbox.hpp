#pragma once

#include <SDL3/SDL.h>
#include "Math.hpp"

class Hitbox
{
    public:
    Hitbox();

    Vector2f getOffset();
    Vector2f getSize();

    void setOffset(Vector2f);
    void setSize(Vector2f);

    private:
    SDL_FRect hitbox; // Hitbox rectangle
};