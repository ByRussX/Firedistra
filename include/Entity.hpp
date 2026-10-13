#pragma once
#include <SDL3/SDL.h>
#include <array>
#include <vector>
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

    bool isIntangible;          // Does its hitbox not collide?
    bool isStatic;              // Is it affected by collisions and physics?
    bool isAxisAligned;         // Can it rotate?
    bool isGrounded;            // On the ground?

    float mass;
    float invMass;
    float inertia;
    float invInertia;

    bool isFlipped;

    Vector2f textureSize;
    SDL_Texture * spriteSheet;
    
    Uint8 rgb[3];
    float colorOffset;

    Entity(const Vector2f& textureSize, SDL_Texture* spriteSheet, const Vector2f& spawnPos, 
           float mass = Config::Physics::DEFAULT_MASS, 
           bool isIntangible = false, 
           bool isStatic = false, 
           bool isAxisAligned = false);
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer) const;
    void renderHitboxes(SDL_Renderer* renderer) const;

    Vector2f getPosition() const;
    void setPosition(const Vector2f & newPos);
    Vector2f getCenter() const;

    void setStatic(bool staticState);
    void setFreezeRotation(bool freezeRot);
    
    void addHitbox(const Vector2f& offset, const Vector2f& size);
    void clearHitboxes();
    std::vector<std::array<Vector2f, 4>> getAllHitboxVertices() const;

    void updateInertia();

private:
    std::vector<Hitbox> hitboxes;
    SDL_FRect getSourceRect() const;
};