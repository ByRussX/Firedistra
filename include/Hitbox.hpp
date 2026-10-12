#pragma once
#include <SDL3/SDL.h>
#include <array>
#include "Math.hpp"

class Hitbox
{
public:
    Hitbox();

    void setOffset(const Vector2f& offset);
    void setSize(const Vector2f& size);
    
    Vector2f getOffset() const;
    Vector2f getSize() const;

    SDL_FRect getAbsoluteRect(const SDL_FRect& entityPos) const;
    std::array<Vector2f, 4> getVertices(const SDL_FRect& entityPos, float angleDeg) const;

private:
    SDL_FRect hitbox;
};