#include "Hitbox.hpp"

Hitbox::Hitbox()
{
    hitbox.x = 0.0f; 
    hitbox.y = 0.0f; 
    hitbox.w = 0.0f; 
    hitbox.h = 0.0f; 
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
    return SDL_FRect{
        entityPos.x + hitbox.x,
        entityPos.y + hitbox.y,
        hitbox.w,
        hitbox.h
    };
}

// Return the absolute position of the vertices of the hitbox on the screen 
std::array<Vector2f, 4> Hitbox::getVertices(const SDL_FRect & entityPos, float angleDeg) const
{
    float halfW = hitbox.w * 0.5f;
    float halfH = hitbox.h * 0.5f;

    // Calculate the center of the hitbox
    Vector2f center(
        entityPos.x + hitbox.x + halfW,
        entityPos.y + hitbox.y + halfH
    );

    // Calculate the vertices when the hitbox is axis aligned
    std::array<Vector2f, 4> localVertices = {
        Vector2f(-halfW, -halfH), 
        Vector2f( halfW, -halfH), 
        Vector2f( halfW,  halfH), 
        Vector2f(-halfW,  halfH)  
    };

    // Rotate the vertices by the angle the hitbox is rotated
    float rad = angleDeg * (3.14159265f / 180.0f);
    std::array<Vector2f, 4> worldVertices;

    for (size_t i = 0; i < 4; i++)
    {
        worldVertices[i] = center + localVertices[i].rotate(rad);
    }

    return worldVertices;
}