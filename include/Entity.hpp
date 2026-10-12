#pragma once
#include <SDL3/SDL.h>
#include <array>
#include "Math.hpp"
#include "Hitbox.hpp"
#include "Config.hpp"

class Entity
{
public:
    SDL_FRect position;
    Vector2f velocity;
    float angle;
    float angularVelocity;
    bool grounded;

    float mass;
    float invMass;
    float inertia;
    float invInertia;

    Hitbox hitbox;
    bool flipped;

    Vector2f textureSize;
    SDL_Texture * spriteSheet;
    
    Uint8 rgb[3];
    float colorOffset;

    Entity(const Vector2f& textureSize, SDL_Texture* spriteSheet, const Vector2f& spawnPos, float mass = Config::Physics::DEFAULT_MASS);
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer) const;
    void renderDebug(SDL_Renderer* renderer) const;

    Vector2f getPosition() const;
    void setPosition(const Vector2f & newPos);
    Vector2f getCenter() const;
    
    void setHitbox(const Vector2f& offset, const Vector2f& size);
    SDL_FRect getAbsoluteHitbox() const;
    std::array<Vector2f, 4> getHitboxVertices() const;

    void updateInertia();

private:
    SDL_FRect getSourceRect() const;
};