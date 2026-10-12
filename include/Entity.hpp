#pragma once
#include <SDL3/SDL.h>
#include <array>
#include "Math.hpp"
#include "Hitbox.hpp"

class Entity
{
public:

    SDL_FRect position;
    Vector2f velocity;
    float angle;
    bool grounded;

    Hitbox hitbox;
    bool flipped;

    Vector2f textureSize;
    SDL_Texture * spriteSheet;
    
    Uint8 rgb[3];
    float colorOffset;

    Entity(const Vector2f& textureSize, SDL_Texture* spriteSheet, const Vector2f& spawnPos);
    
    void update();
    void render(SDL_Renderer* renderer) const;

    Vector2f getPosition() const;
    void setPosition(const Vector2f & newPos);
    
    void setHitbox(const Vector2f& offset, const Vector2f& size);
    SDL_FRect getAbsoluteHitbox() const;
    std::array<Vector2f, 4> getHitboxVertices() const;

private:
    SDL_FRect getSourceRect() const;
};