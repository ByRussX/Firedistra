#pragma once
#include <SDL3/SDL.h>
#include <array>
#include "Math.hpp"

class Hitbox
{
public:
    Hitbox(const Vector2f& offset = Vector2f(0.0f, 0.0f), const Vector2f& size = Vector2f(0.0f, 0.0f));

    void setOffset(const Vector2f& offset);
    void setSize(const Vector2f& size);
    
    Vector2f getOffset() const;
    Vector2f getSize() const;

    SDL_FRect getAbsoluteRect(const SDL_FRect& entityPos) const;
    std::array<Vector2f, 4> getVertices(const SDL_FRect& entityPos, float angleDeg, bool isFlipped) const;

private:
    SDL_FRect hitbox;
};