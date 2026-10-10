#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>
#include <array>

#include "Utils.hpp"
#include "Math.hpp"
#include "AmogusState.hpp"
#include "Hitbox.hpp"
#include "Config.hpp"

class Entity
{
public:
    Entity(const Vector2f & p_textureSize, SDL_Texture * p_spriteSheet, const Vector2f & p_screenPos);

    void changeHitbox(const Vector2f & offset, const Vector2f & size);
    Vector2f getHitboxSize() const;
    Vector2f getHitboxOffset() const;

    void update(bool moved, float deltaTime);

    void physicsEffects(float deltaTime);
    void calculateCollisions(float deltaTime);
    void resolveEntityCollision(Entity & other);

    void entityRenderer(SDL_Renderer * renderer);

    void changeState(AmogusState newState);

    const SDL_FRect & getScreenPos() const;
    SDL_FRect & getScreenPos();
    SDL_FRect getAbsoluteHitbox() const;

    std::array<Vector2f, 4> getHitboxVertices() const;

    void setPosX(float newX);
    void setPosY(float newY);

    void addVelX(float velX);
    void addVelY(float velY);

    bool isGrounded() const;

    Vector2f entityVelocity{0.0f, 0.0f};

    void changeRGB(Uint8 r, Uint8 g, Uint8 b);
    void addRGB(int r, int g, int b);

    SDL_FRect getSourceRect(int col = 0, int row = 0) const;

private:
    SDL_FRect entityPosition{0.0f, 0.0f, 0.0f, 0.0f};
    float entityAngle = 0.0f; // Entity angle in degrees

    bool grounded = false;
    Hitbox entityHitbox;

    Vector2f textureSize{0.0f, 0.0f};
    SDL_Texture * spriteSheet = nullptr;

    AmogusState currentState = AmogusState::RIGHT;

    Uint8 rgb[3]{255, 255, 255};
    float colorOffset = 0.0f;
};