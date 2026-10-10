#include "Hitbox.hpp"

Hitbox::Hitbox()
{
    hitbox.x = 0.0f; // X offset from the leftmost corner of the entity
    hitbox.y = 0.0f; // Y offset from the leftmost corner of the entity
    hitbox.w = 0.0f; // Width of the hitbox
    hitbox.h = 0.0f; // Height of the hitbox
}

Vector2f Hitbox::getOffset() const
{
    return Vector2f(hitbox.x, hitbox.y);
}

Vector2f Hitbox::getSize() const
{
    return Vector2f(hitbox.w, hitbox.h);
}

void Hitbox::setOffset(const Vector2f & offset)
{
    hitbox.x = offset.x;
    hitbox.y = offset.y;
}

void Hitbox::setSize(const Vector2f & size)
{
    hitbox.w = size.x;
    hitbox.h = size.y;
}
