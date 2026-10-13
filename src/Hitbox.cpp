#include "Hitbox.hpp"

Hitbox::Hitbox(const Vector2f& offset, const Vector2f& size)
{
    hitbox.x = offset.x; 
    hitbox.y = offset.y; 
    hitbox.w = size.x; 
    hitbox.h = size.y; 
}

// Getter for offset
Vector2f Hitbox::getOffset() const { return Vector2f(hitbox.x, hitbox.y); }

// Getter for size
Vector2f Hitbox::getSize() const { return Vector2f(hitbox.w, hitbox.h); }

// Set the hitbox offset (top left corner)
void Hitbox::setOffset(const Vector2f& offset)
{
    hitbox.x = offset.x;
    hitbox.y = offset.y;
}

// Set the hitbox size (offset to the bottom right corner)
void Hitbox::setSize(const Vector2f & size)
{
    hitbox.w = size.x;
    hitbox.h = size.y;
}

// Return the absolute position of the hitbox on the screen as an AABB rectangle
SDL_FRect Hitbox::getAbsoluteRect(const SDL_FRect & entityPos) const
{
    return SDL_FRect
    {
        entityPos.x + hitbox.x, 
        entityPos.y + hitbox.y, 
        hitbox.w, 
        hitbox.h
    };
}

// Return the absolute position of the vertices of the hitbox on the screen 
std::array<Vector2f, 4> Hitbox::getVertices(const SDL_FRect& entityPos, float angleDeg, bool isFlipped) const
{
    // Half of the width and height of the hitbox
    float halfW = hitbox.w * 0.5f;
    float halfH = hitbox.h * 0.5f;

    // Calculate the local center of the hitbox
    Vector2f localCenter(
        hitbox.x + halfW, 
        hitbox.y + halfH
    );

    // Calculate the global center of the entity (rotation center)
    Vector2f entityCenter(
        entityPos.x + entityPos.w * 0.5f, 
        entityPos.y + entityPos.h * 0.5f
    );

    // If flipped, mirror the local center in the x axis
    if (isFlipped) 
    {
        localCenter.x = entityPos.w - localCenter.x;
    }

    // Vector from the rotation center to the hitbox center
    Vector2f offsetFromCenter(
        localCenter.x - entityPos.w * 0.5f,
        localCenter.y - entityPos.h * 0.5f
    );

    // Rotate the offset by the angle the entity is rotated (in radians)
    Vector2f rotatedOffset = offsetFromCenter.rotate(angleDeg * Math::DEG_TO_RAD);

    // Final global center of the hitbox
    Vector2f globalHitboxCenter = entityCenter + rotatedOffset;

    // Calculate the vertices when the hitbox is axis aligned
    std::array<Vector2f, 4> localVertices = {
        Vector2f(-halfW, -halfH), 
        Vector2f( halfW, -halfH), 
        Vector2f( halfW,  halfH), 
        Vector2f(-halfW,  halfH)  
    };

    // Rotate the vertices around the hitbox center
    std::array<Vector2f, 4> worldVertices;
    for (size_t i = 0; i < 4; i++)
    {
        worldVertices[i] = globalHitboxCenter + localVertices[i].rotate(angleDeg * Math::DEG_TO_RAD);
    }

    return worldVertices;
}