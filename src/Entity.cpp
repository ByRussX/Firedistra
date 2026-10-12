#include "Entity.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include <cmath>
#include <algorithm>

Entity::Entity(const Vector2f & textureSize, SDL_Texture * spriteSheet, const Vector2f & spawnPos)
    : velocity(0.0f, 0.0f),
      angle(0.0f),
      grounded(false),
      flipped(false),
      textureSize(textureSize),
      spriteSheet(spriteSheet),
      colorOffset(static_cast<float>(rand() % 100))
{
    // Inicializar la posición y el tamaño en pantalla (escalado)
    position.x = spawnPos.x;
    position.y = spawnPos.y;
    position.w = textureSize.x * Config::Entity::SCALE;
    position.h = textureSize.y * Config::Entity::SCALE;

    // Color blanco por defecto
    rgb[0] = 255;
    rgb[1] = 255;
    rgb[2] = 255;
}

// Update entity visuals
void Entity::update()
{
    // Rotate 1 degree per frame
    angle += 1.0f;
    if (angle >= 360.0f)
    {
        angle -= 360.0f;
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
    
    // Set the texture flip state
    SDL_FlipMode flip = (flipped) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_FRect texturePart = getSourceRect();

    SDL_SetTextureColorMod(spriteSheet, rgb[0], rgb[1], rgb[2]);
    SDL_RenderTextureRotated(renderer, spriteSheet, &texturePart, &position, angle, nullptr, flip);
}

// Transforms and physics

// Get entity position
Vector2f Entity::getPosition() const
{
    return Vector2f(position.x, position.y);
}

// Set entity position
void Entity::setPosition(const Vector2f & newPos)
{
    position.x = newPos.x;
    position.y = newPos.y;
}

// Get absolute position for the hitbox vertices on screen
std::array<Vector2f, 4> Entity::getHitboxVertices() const
{
    return hitbox.getVertices(position, angle);
}

// Set the hitbox for the entity
void Entity::setHitbox(const Vector2f& offset, const Vector2f& size)
{
    hitbox.setOffset(Vector2f(offset.x * Config::Entity::SCALE, offset.y * Config::Entity::SCALE));
    hitbox.setSize(Vector2f(size.x * Config::Entity::SCALE, size.y * Config::Entity::SCALE));
}

// Get absolute position for the hitbox on screen
SDL_FRect Entity::getAbsoluteHitbox() const
{
    return hitbox.getAbsoluteRect(position);
}

// Get 
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