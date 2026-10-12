#include "Entity.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include <cmath>
#include <algorithm>

Entity::Entity(const Vector2f & textureSize, SDL_Texture * spriteSheet, const Vector2f & spawnPos, float mass, bool isStatic, bool freezeRotation)
    : velocity(0.0f, 0.0f),
      angle(0.0f),
      angularVelocity(0.0f),
      grounded(false),
      isStatic(isStatic),
      freezeRotation(freezeRotation),
      mass(mass),
      invMass(0.0f),
      inertia(1.0f),
      invInertia(1.0f),
      flipped(false),
      textureSize(textureSize),
      spriteSheet(spriteSheet),
      colorOffset(static_cast<float>(rand() % 100))
{
    // Initialize position and dimensions (scaled)
    position.x = spawnPos.x;
    position.y = spawnPos.y;
    position.w = textureSize.x * Config::Entity::SCALE;
    position.h = textureSize.y * Config::Entity::SCALE;

    updateInertia();

    // Default white color
    rgb[0] = 255;
    rgb[1] = 255;
    rgb[2] = 255;
}

// Recalculates moment of inertia for a rectangular rigid body: I = (1/12) * m * (w^2 + h^2)
void Entity::updateInertia()
{
    Vector2f size = hitbox.getSize();
    float w = size.x > 0.0f ? size.x : position.w;
    float h = size.y > 0.0f ? size.y : position.h;

    if (!isStatic && mass > 0.0f)
    {
        invMass = 1.0f / mass;
        inertia = (1.0f / 12.0f) * mass * (w * w + h * h);
        invInertia = (!freezeRotation && inertia > 0.0f) ? (1.0f / inertia) : 0.0f;
    }
    else
    {
        invMass = 0.0f;
        inertia = 0.0f;
        invInertia = 0.0f;
        velocity = Vector2f(0.0f, 0.0f);
        angularVelocity = 0.0f;
    }

    if (freezeRotation)
    {
        invInertia = 0.0f;
        angularVelocity = 0.0f;
    }
}

// Update entity visuals and integrate rotational angle
void Entity::update(float deltaTime)
{
    // Rotate dynamically according to physical angular velocity only if rotation is not frozen and entity is not static
    if (!freezeRotation && !isStatic)
    {
        angle += (angularVelocity * Math::RAD_TO_DEG) * deltaTime;
        
        // Normalize angle into [0, 360)
        angle = std::fmod(angle, 360.0f);
        if (angle < 0.0f)
        {
            angle += 360.0f;
        }
    }
    else
    {
        angularVelocity = 0.0f;
    }

    // Change sprite color using sine waves
    float t = static_cast<float>(Utils::getTimeInSeconds() * 2.0 + colorOffset);

    rgb[0] = static_cast<Uint8>((std::sin(t) + 1.0f) * 127.5f);
    rgb[1] = static_cast<Uint8>((std::sin(t + 2.0f) + 1.0f) * 127.5f);
    rgb[2] = static_cast<Uint8>((std::sin(t + 4.0f) + 1.0f) * 127.5f);
}

// Render the entity
void Entity::render(SDL_Renderer* renderer) const
{
    if (renderer == nullptr || spriteSheet == nullptr) return;
    
    // Set texture flip state
    SDL_FlipMode flip = (flipped) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_FRect texturePart = getSourceRect();

    SDL_SetTextureColorMod(spriteSheet, rgb[0], rgb[1], rgb[2]);
    SDL_RenderTextureRotated(renderer, spriteSheet, &texturePart, &position, angle, nullptr, flip);
}

// Render the hitbox
void Entity::renderDebug(SDL_Renderer* renderer) const
{
    if (renderer == nullptr) return;

    // Obtain the four vertices in global coordinates
    auto verts = getHitboxVertices();

    // Render in green to make it notable
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    // Draw the four edges connecting the vertices
    for (size_t i = 0; i < 4; ++i)
    {
        const Vector2f & p1 = verts[i];
        const Vector2f & p2 = verts[(i + 1) % 4];

        SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }
}

// Transforms and physics

// Get entity position (top-left)
Vector2f Entity::getPosition() const
{
    return Vector2f(position.x, position.y);
}

// Set entity position (top-left)
void Entity::setPosition(const Vector2f & newPos)
{
    position.x = newPos.x;
    position.y = newPos.y;
}

// Get center of mass of the entity
Vector2f Entity::getCenter() const
{
    Vector2f offset = hitbox.getOffset();
    Vector2f size = hitbox.getSize();
    return Vector2f(position.x + offset.x + size.x * 0.5f,
                    position.y + offset.y + size.y * 0.5f);
}

// Set whether the entity is static (fixed in place) or dynamic
void Entity::setStatic(bool staticState)
{
    isStatic = staticState;
    if (isStatic)
    {
        velocity = Vector2f(0.0f, 0.0f);
        angularVelocity = 0.0f;
    }
    updateInertia();
}

// Set whether the entity's rotation is locked/frozen
void Entity::setFreezeRotation(bool freezeRot)
{
    freezeRotation = freezeRot;
    if (freezeRotation)
    {
        angularVelocity = 0.0f;
    }
    updateInertia();
}

// Get absolute position for the hitbox vertices on screen
std::array<Vector2f, 4> Entity::getHitboxVertices() const
{
    return hitbox.getVertices(position, angle);
}

// Set hitbox for the entity and recalculate inertia
void Entity::setHitbox(const Vector2f& offset, const Vector2f& size)
{
    hitbox.setOffset(Vector2f(offset.x * Config::Entity::SCALE, offset.y * Config::Entity::SCALE));
    hitbox.setSize(Vector2f(size.x * Config::Entity::SCALE, size.y * Config::Entity::SCALE));
    updateInertia();
}

// Get absolute position for the hitbox on screen
SDL_FRect Entity::getAbsoluteHitbox() const
{
    return hitbox.getAbsoluteRect(position);
}

// Get source sprite rect
SDL_FRect Entity::getSourceRect() const
{
    return SDL_FRect
    {
        0.0f,
        0.0f,
        textureSize.x,
        textureSize.y
    };
}